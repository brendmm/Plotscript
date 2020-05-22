#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <complex>
#include <QWidget>
#include <QPlainTextEdit>

class QPlainTextEdit;

class InputWidget : public QPlainTextEdit {
	Q_OBJECT
public:
	InputWidget(QPlainTextEdit * parent = nullptr);
	void evaluate(QString command);
signals:
	void changed(QString command);

protected:

	void keyPressEvent(QKeyEvent *event);

private:
	QPlainTextEdit * textEdit;
	QString * myInput;
};
#endif