#ifndef CONSUMER_HPP
#define CONSUMER_HPP


/************************************************************************************************************************/
/*							THIS DERIVED AND EDITED FROM AN EXAMPLE IN ONE OF THE LECTURES								*/
/*																														*/
/*										CREDIT GIVEN TO PROFESSOR WYATT													*/
/************************************************************************************************************************/


#include <csignal>
#include <cstdlib>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <string>
#include "interpreter.hpp"
#include "semantic_error.hpp"

class Consumer
{
public:
	Consumer(ThreadSafeQueue<std::string> *stringQueuePtr, ThreadSafeQueue<Expression> *expressionQueuePtr, Interpreter *theinterp)
	{
		interp = theinterp;
		stringQueue = stringQueuePtr;
		expressionQueue = expressionQueuePtr;
		eval_startup(*interp);
	}

	int eval_startup(Interpreter &interp) {
		Atom error(false);
		std::ifstream ifs(STARTUP_FILE);
		if (!ifs) {
			std::string error("error");
			std::string stringErrorMessage("Error: Invalid Startup.");
			error.append(stringErrorMessage);
			Atom errorMessage(error);
			expressionQueue->push(Expression(Expression(errorMessage)));
			return EXIT_FAILURE;
		}
		if (!interp.parseStream(ifs)) {
			std::string error("error");
			std::string stringErrorMessage("Error: Invalid Startup. Could not parse.");
			error.append(stringErrorMessage);
			Atom errorMessage(error);
			expressionQueue->push(Expression(Expression(errorMessage)));
			return EXIT_FAILURE;
		}

		else {
			try {
				Expression exp = interp.evaluate();
			}
			catch (const SemanticError & ex) {
				std::cerr << ex.what() << std::endl;
				expressionQueue->push(Expression(error));
				return EXIT_FAILURE;
			}
		}
		return EXIT_SUCCESS;
	}

	void operator()()
	{
		Expression exp;
		Atom error(false);
		bool stopper = false;
		while (!stopper) {

			std::string myString;
			stringQueue->wait_and_pop(myString);

			Environment tempEnv = interp->getEnv();
			std::istringstream expression(myString);
			if (myString == "%start") {
			}
			else if (myString == "%stop") {
				stopper = true;
			}
			else if (myString == "%reset") {
			}
			else if (myString == "%exit") {
			}

			else if (!interp->parseStream(expression)) {
				std::string error("error");
				std::string stringErrorMessage("Error: Invalid Expression. Could not parse.");
				error.append(stringErrorMessage);
				Atom errorMessage(error);
				expressionQueue->push(Expression(Expression(errorMessage)));
			}

			else {
				try {
					exp = interp->evaluate();
					expressionQueue->push(exp);
				}
				catch (const SemanticError & ex) {
					interp->setEnv(tempEnv);
					std::string error("error");
					std::string stringErrorMessage(ex.what());
					error.append(stringErrorMessage);
					Atom errorMessage(error);
					expressionQueue->push(Expression(Expression(errorMessage)));
				}
			}
		}
		std::string myString;
		if (stringQueue->try_pop(myString)) {
			std::istringstream expression(myString);
			if (myString == "%start") {
				stopper = false;
				expressionQueue->push(Expression(error));
			}
		}
	}

private:

	Interpreter * interp;
	ThreadSafeQueue<std::string> * stringQueue;
	ThreadSafeQueue<Expression> * expressionQueue;

};
#endif