
#include "notebook_app.hpp"

#include <QPlainTextEdit>
#include <QLineEdit>
#include <QLayout>
#include <cmath>
#include <QDebug>
#include <QTime>
#include <QTimer>


NotebookApp::NotebookApp(QWidget * parent) : QWidget (parent) {

	input = new InputWidget();
	input->setObjectName("input");
	output = new OutputWidget();
	output->setObjectName("output");

	startButton = new QPushButton("Start Kernel");
	startButton->setObjectName("start");
	stopButton = new QPushButton("Stop Kernel");
	stopButton->setObjectName("stop");
	resetButton = new QPushButton("Reset Kernel");
	resetButton->setObjectName("reset");
	interruptButton = new QPushButton("Interrupt");
	interruptButton->setObjectName("interrupt");

	QObject::connect(startButton, SIGNAL(clicked()), this, SLOT(handleStart()));
	QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(handleStop()));
	QObject::connect(resetButton, SIGNAL(clicked()), this, SLOT(handleReset()));
	QObject::connect(interruptButton, SIGNAL(clicked()), this, SLOT(handleInterrupt()));

	QObject::connect(input, &InputWidget::changed, this, &NotebookApp::realChange);
	QObject::connect(this, &NotebookApp::sendOutput, output, &OutputWidget::realChange);

	auto layout = new QGridLayout();
	auto buttonLayout = new QGridLayout();

	buttonLayout->addWidget(startButton, 0, 0);
	buttonLayout->addWidget(stopButton, 0, 1);
	buttonLayout->addWidget(resetButton, 0, 2);
	buttonLayout->addWidget(interruptButton, 0, 3);
	layout->addLayout(buttonLayout, 0, 0);
	layout->addWidget(input, 1, 0);
	layout->addWidget(output, 2, 0);


	setLayout(layout);

	stringQueue = new ThreadSafeQueue<std::string>();
	expressionQueue = new ThreadSafeQueue<Expression>();
	interp = new Interpreter();
	myInput = new Consumer(stringQueue, expressionQueue, interp);
	consumer_th1 = new std::thread(*myInput);
	kernelStatus = true;

}

NotebookApp::~NotebookApp() {						//ends thread, join, delete everything

	realChange("%stop");
	while (!consumer_th1->joinable()) {}
	consumer_th1->join();
	delete stringQueue;
	delete expressionQueue;
	delete interp;
	delete myInput;
	delete input;
	delete output;
	

}

void NotebookApp::realChange(QString command) {						//recieves command from input
	if (kernelStatus) {												//if running it will push to queue and go to loop to pop
		value = command.toStdString();
		stringQueue->push(value);
		QTimer::singleShot(0, this, SLOT(myLoop()));
	}
	else {
		output->displayError("Error: interpreter kernel not running"); //not running just throws error message
	}
}

void NotebookApp::myLoop() {									//only running when on and recieved messge
	exp = new Expression();
	QTime t;
	t.start();
		
	if (expressionQueue->try_pop(*exp)) {						//try pop
		interp->resetIntInterrupt();							//make sure eval will not throw error
		emit sendOutput(*exp);									//output
	}

	delete exp;
	QTimer::singleShot(0, this, SLOT(myLoop()));				//calls itself
}
	
void  NotebookApp::handleStart() {								//starts new thread
	if (!kernelStatus) {
		consumer_th1 = new std::thread(*myInput);
	}
	kernelStatus = true;
}
void  NotebookApp::handleStop() {								//ends and joins thread
	if (kernelStatus) {
		stringQueue->push("%stop");
		while (!consumer_th1->joinable()) {}
		consumer_th1->join();
		delete consumer_th1;
		Expression tempExp;
	}

	kernelStatus = false;
}
void  NotebookApp::handleReset() {								//essentuially starts and stops
	if (kernelStatus) {
		stringQueue->push("%stop");
		while (!consumer_th1->joinable()) {}
		consumer_th1->join();
		delete consumer_th1;
		delete myInput;
		delete interp;
	}

	interp = new Interpreter();
	myInput = new Consumer(stringQueue, expressionQueue, interp);
	consumer_th1 = new std::thread(*myInput);
	stringQueue->push("%start");

	kernelStatus = true;
}
		
void  NotebookApp::handleInterrupt() {						//stops eval from working
	if (kernelStatus) {
		interp->throwIntInterrupt();
	}
}