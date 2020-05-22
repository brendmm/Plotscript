#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include "handleInterrupt.hpp"
#include "startup_config.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "threadQueue.hpp"
#include "consumer.hpp"

void prompt() {
	std::cout << "\nplotscript> ";
}

std::string readline() {

	std::string line;
	std::getline(std::cin, line);

	//Catch Interrupt at Prompt
	if (std::cin.fail() || std::cin.eof()) {
		std::cin.clear(); // reset cin state
		line.clear(); //clear input string
		std::cout << "Error: interpreter kernel interrupted" << std::endl;
	}
	global_status_flag = 0;
	return line;
}

void error(const std::string & err_str) {
	std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str) {
	std::cout << "Info: " << err_str << std::endl;
}

int eval_startup(Interpreter &interp) {

	std::ifstream ifs(STARTUP_FILE);
	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}

	if (!interp.parseStream(ifs)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}

	else {
		try {
			Expression exp = interp.evaluate();
		}
		catch (const SemanticError & ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int eval_from_stream(std::istream & stream) {

	Interpreter interp;
	eval_startup(interp);
	if (!interp.parseStream(stream)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}
	else {
		try {
			Expression exp = interp.evaluate();
			std::cout << exp << std::endl;
		}
		catch (const SemanticError & ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int eval_from_file(std::string filename) {

	std::ifstream ifs(filename);
	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}
	return eval_from_stream(ifs);
}

int eval_from_command(std::string argexp) {

	std::istringstream expression(argexp);
	return eval_from_stream(expression);
}

std::string cleanInput(std::string &line) {		//will continuously prompt if the input is start or empty line

	line.clear();
	while (line == "") {
		line = "%start";
		while (line == "%start") {
			prompt();
			line = readline();
		}
	}
	return line;
}

// A REPL is a repeated read-eval-print loop
void repl() {

	std::string line;	
	//initially declare thread queues and intial thread
	ThreadSafeQueue<std::string> * stringQueue = new ThreadSafeQueue<std::string>();
	ThreadSafeQueue<Expression> * expressionQueue = new ThreadSafeQueue<Expression>();
	Interpreter * interp = new Interpreter();
	Consumer * input = new Consumer(stringQueue, expressionQueue, interp);
	std::thread * consumer_th1 = new std::thread(*input);
	bool activeKernel = true;			//start kernel running
	line = cleanInput(line);		//wait for first input
	stringQueue->push(line);

	while (!std::cin.eof()) {

		if (line == "%exit") {		
			if (activeKernel == true) {						//delete thread if kernel is on
				stringQueue->push("%stop");
				while (!consumer_th1->joinable()) {}
				consumer_th1->join();
				delete consumer_th1;
			}

			Expression tempExp;									//delete everything else and break
			while (expressionQueue->try_pop(tempExp)) {}
			while (stringQueue->try_pop(line)) {}
			delete stringQueue;
			delete expressionQueue;
			delete input;
			delete interp;
			break;
		}

		if (line == "%start" && activeKernel == false) {				//only if kernel is off, initiate new thread and wait input
			consumer_th1 = new std::thread(*input);
			activeKernel = true;
			line = cleanInput(line);
			if (line.empty() || line == "%stop") continue;
			stringQueue->push(line);
		}

		if (line == "%stop" && activeKernel == true) {					//only if kernel is on, delete evertyhing
			stringQueue->push(line);
			while (!consumer_th1->joinable()) {}
			consumer_th1->join();
			delete consumer_th1;
			Expression tempExp;
			while (expressionQueue->try_pop(tempExp)) {}
			activeKernel = false;
		}


		if (line == "%reset") {													//essentially will stop and start
			if (activeKernel == true) {
				stringQueue->push("%stop");
				while (!consumer_th1->joinable()) {}
				consumer_th1->join();
				delete consumer_th1;
				Expression tempExp;
				while (expressionQueue->try_pop(tempExp)) {}
			}

			interp = new Interpreter();
			input = new Consumer(stringQueue, expressionQueue, interp);
			consumer_th1 = new std::thread(*input);
			activeKernel = true;
			line = cleanInput(line);
			if (line.empty() || line == "%stop") continue;
			stringQueue->push(line);
		}

		if (activeKernel == true) {													//kernel is on. try pop and if it works output and reprompt
			Expression exp;
			if (expressionQueue->try_pop(exp)) {

				std::cout << exp << std::endl;

				line = cleanInput(line);
				if (line.empty() || line == "%stop") continue;
				stringQueue->push(line);
			}
		}
		if (activeKernel == false) {											//just display error message
			prompt();
			line = readline();
			if (line.empty() || line == "%start" || line == "%reset" || line == "%exit") continue;
			std::cout << "Error: interpreter kernel not running" << std::endl;
		}

		if (global_status_flag > 0) {												//register interrupt
			global_status_flag = 0;
			if (activeKernel == true) {												//trigger eval to stop working
				interp->throwIntInterrupt();
				Expression temp;
				expressionQueue->wait_and_pop(temp);								//return error message
				std::cout << temp << std::endl;
				line = cleanInput(line);
				interp->resetIntInterrupt();
				if (line.empty() || line == "%stop") continue;
				stringQueue->push(line);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	install_handler();
	if (argc == 2) {
		return eval_from_file(argv[1]);
	}
	else if (argc == 3) {
		if (std::string(argv[1]) == "-e") {
			return eval_from_command(argv[2]);
		}
		else {
			error("Incorrect number of command line arguments.");
		}
	}
	else {
		repl();
	}
	return EXIT_SUCCESS;
}