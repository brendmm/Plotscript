
#include "input_widget.hpp"
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QLayout>
#include <cmath>
#include <QString>
#include <QDebug>

InputWidget::InputWidget(QPlainTextEdit * parent) : QPlainTextEdit (parent) {
	textEdit = new QPlainTextEdit();
}

void  InputWidget::evaluate(QString command) {
	emit changed(command);
}


void InputWidget::keyPressEvent(QKeyEvent *event) {
	if ((event->modifiers() == Qt::ShiftModifier) && ((event->key() == Qt::Key_Return) || (event->key() == Qt::Key_Enter))) {
		evaluate(this->toPlainText());
		event->accept();
	}
	else {
		QPlainTextEdit::keyPressEvent(event);
	}
}