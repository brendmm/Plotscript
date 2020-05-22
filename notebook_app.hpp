#ifndef COMPLEX_WIDGET_H
#define COMPLEX_WIDGET_H

#include <complex>
#include <QWidget>
#include <thread>
#include <QPushButton>
#include "input_widget.hpp"
#include "output_widget.hpp"
#include "threadQueue.hpp"
#include "consumer.hpp"

class NotebookApp : public QWidget {
	Q_OBJECT
public:

	NotebookApp(QWidget * parent = nullptr);
	~NotebookApp();
public slots:
	void realChange(QString command);
	void myLoop();
	void handleStart();
	void handleStop();
	void handleReset();
	void handleInterrupt();

signals:
	void sendOutput(Expression exp);

private:


	InputWidget * input;
	OutputWidget * output;
	std::string value;
	bool stopper;
	ThreadSafeQueue<std::string> * stringQueue;
	ThreadSafeQueue<Expression> * expressionQueue;
	Interpreter * interp;
	Expression * exp;
	Consumer * myInput;
	std::thread * consumer_th1;

	QPushButton *  interruptButton;
	QPushButton *  resetButton;
	QPushButton *  stopButton;
	QPushButton *  startButton;
	bool kernelStatus;


};
#endif
