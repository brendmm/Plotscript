#include "output_widget.hpp"
#include "interpreter.hpp"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QLayout>
#include <cmath>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <thread>


QString OutputWidget::output_Atom_as_qstring(Atom a) {

	QString myOutput;
	if (a.isNumber()) {
		myOutput.append(QString::number(a.asNumber(), 'g', 6));
	}
	if (a.isSymbol()) {
		myOutput.append(QString::fromStdString(a.asSymbol()));
	}
	if (a.isComplex()) {
		myOutput.append("(");
		myOutput.append(QString::number(a.asComplex().real(), 'g', 6));
		myOutput.append(",");
		myOutput.append(QString::number(a.asComplex().imag(), 'g', 6));
		myOutput.append(")");
	}
	if (a.isList()) {

	}
	if (a.isLambda()) {
		myOutput.append(QString::fromStdString(a.asLambda()));
	}
	if (a.isString()) {
		myOutput.append(QString::fromStdString(a.asString()));
	}
	if (a.isError()) {
		myOutput.append(QString::fromStdString(a.asError()));
	}
	return myOutput;
}

void OutputWidget::output_Expression_as_qstring(Expression exp) {
	Environment env;
	QString myOutput;
	Atom propertyCheck = Atom(std::string("\"object-name\""));
	Expression myPropertyValue;
	if (exp.checkProperty(propertyCheck)) {
		myPropertyValue = exp.getProperty(propertyCheck);
		if (myPropertyValue.head().asString() == "\"point\"") {
			this->displayPoint(exp);
		}
		else if (myPropertyValue.head().asString() == "\"line\"") {
			displayLine(exp);
		}
		else if (myPropertyValue.head().asString() == "\"text\"") {
			displayTextAtLocation(exp);
		}
	}
	else {
		if (!exp.head().isComplex() && !exp.head().isNone() && !exp.head().isLambda() && !exp.head().isList() && !exp.head().isError()) {
			myOutput.append("(");
		}
		if (!exp.head().isList() && !exp.head().isLambda() && !exp.head().isNone()) {
			myOutput.append(output_Atom_as_qstring(exp.head()));
			if (env.is_proc(exp.head()) || exp.head().isLambda()) {
				myOutput.append(" ");
			}
		}
		if (exp.head().isNone()) {
			myOutput.append("NONE");
		}

		if (!exp.head().isLambda()) {
			Expression temp;
			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
				temp = *e;
				output_Expression_as_qstring(temp);
			}
		}
		if (!exp.head().isComplex() && !exp.head().isNone() && !exp.head().isLambda() && !exp.head().isList() && !exp.head().isError()) {
			myOutput.append(")");
		}
		if (!exp.isHeadLambda() && !exp.isHeadList()) {
			displayText(myOutput);
		}
	}
}

void OutputWidget::displayText(QString myString) {
	myScene->addText(myString);
	myText = myString;
}

void OutputWidget::displayError(QString myString) {
	myScene->clear();
	myScene->addText(myString);
	myText = myString;
	myView->fitInView(myScene->sceneRect(), Qt::KeepAspectRatio);
}

QString OutputWidget::getText() {
	return myText;
}

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
	
	myScene = new QGraphicsScene();
	myView = new QGraphicsView();
	myView->setScene(myScene);
	auto layout = new QGridLayout();
	layout->addWidget(myView, 0, 0);
	setLayout(layout);
}

void OutputWidget::realChange(Expression exp) {

	myScene->clear();
	output_Expression_as_qstring(exp);
	myView->fitInView(myScene->sceneRect(), Qt::KeepAspectRatio);

}

void  OutputWidget::resizeEvent(QResizeEvent *) {
	myView->fitInView(myScene->sceneRect(), Qt::KeepAspectRatio);
}

void OutputWidget::displayPoint(Expression exp) {
	std::string size = "\"size\"";
	Atom mySize = size;
	if (exp.getTailLength() != 2) {
		displayText(QString("Point does not have correct number of coordinates"));
	}
	else if (!exp.getValueInTail(0).head().isNumber() || !exp.getValueInTail(1).head().isNumber()) {
		displayText(QString("One or more coordinates are not numbers"));
	}

	else if (!exp.checkProperty(mySize)) {
		displayText(QString("Point has no property for size"));
	}
	else if (!exp.getProperty(mySize).head().isNumber()) {
		displayText(QString("Size property is not number"));
	}
	else if (exp.getProperty(mySize).head().asNumber() < 0) {
		displayText(QString("Size property is less than 0"));
	}
	else {
		double sizeOffest = exp.getProperty(mySize).head().asNumber() / 2;
		QPointF myCenterPoint(exp.getValueInTail(0).head().asNumber() - sizeOffest, exp.getValueInTail(1).head().asNumber() - sizeOffest);
		QSizeF myPointSize(exp.getProperty(mySize).head().asNumber(), exp.getProperty(mySize).head().asNumber());
		QRectF myCoordinates(myCenterPoint, myPointSize);
		QPen myPen(Qt::NoPen);
		QBrush myBrush(Qt::black);
		myScene->addEllipse(myCoordinates, myPen, myBrush);
	}
}

void OutputWidget::displayLine(Expression exp) {
	std::string thickness = "\"thickness\"";
	Atom myThickness = thickness;
	if (exp.getTailLength() != 2) {
		displayText(QString("Make-Line does not have correct number of points"));
	}
	else if (!exp.getValueInTail(0).isHeadList() || !exp.getValueInTail(1).isHeadList()) {
		displayText(QString("One or more expected points are do not have coordinates"));
	}
	else if ((exp.getValueInTail(0).getTailLength() != 2) || (exp.getValueInTail(1).getTailLength() != 2)) {
		displayText(QString("One or more expected points do not have correct number of coordinates"));
	}
	else if (!exp.getValueInTail(0).getValueInTail(0).head().isNumber() || !exp.getValueInTail(0).getValueInTail(1).head().isNumber()){ 
		displayText(QString("One or more coordinates of point 1 are not numbers"));
	}
	else if (!exp.getValueInTail(1).getValueInTail(0).head().isNumber() || !exp.getValueInTail(1).getValueInTail(1).head().isNumber()) {
		displayText(QString("One or more coordinates of point 2 are not numbers"));
	}

	else if (!exp.checkProperty(myThickness)) {
		displayText(QString("Point has no property for thickness"));
	}
	else if (!exp.getProperty(myThickness).head().isNumber()) {
		displayText(QString("Thickness property is not number"));
	}
	else if (exp.getProperty(myThickness).head().asNumber() < 0) {
		displayText(QString("Thickness property is less than 0"));
	}
	else {
		QPointF myPoint1(exp.getValueInTail(0).getValueInTail(0).head().asNumber(), exp.getValueInTail(0).getValueInTail(1).head().asNumber());
		QPointF myPoint2(exp.getValueInTail(1).getValueInTail(0).head().asNumber(), exp.getValueInTail(1).getValueInTail(1).head().asNumber());
		myLineFirstPoint = myPoint1;
		myLineSecondPoint = myPoint2;
		QLineF myLine(myPoint1, myPoint2);
		QPen myPen(Qt::black, exp.getProperty(myThickness).head().asNumber(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		lineThickness = exp.getProperty(myThickness).head().asNumber();
		myScene->addLine(myLine, myPen);
	}
}

void OutputWidget::displayTextAtLocation(Expression exp) {
	std::string position = "\"position\"";
	Atom myPosition = position;
	std::string size = "\"size\"";
	Atom mySize = size;
	if (exp.getTailLength() != 0) {
		displayText(QString("make-text has been given too many parameters"));
	}
	else if (!exp.head().isString()) {
		displayText(QString("make-text expected a string value"));
	}
	else if (!exp.checkProperty(myPosition)) {
		displayText(QString("Point has no property for positoin"));
	}

	else if (!validPoint(exp.getProperty(myPosition))) {

	}

	else {

		double myScaleFactor = checkScale(exp);
		double myAngle = checkRotation(exp);

		qreal myQx(exp.getProperty(myPosition).getValueInTail(0).head().asNumber());
		qreal myQy(exp.getProperty(myPosition).getValueInTail(1).head().asNumber());
		QGraphicsTextItem *myText = myScene->addText(QString::fromStdString((exp.head().asString().substr(1, exp.head().asString().size() -2))));
		myLocationText = QString::fromStdString((exp.head().asString().substr(1, exp.head().asString().size() - 2)));

		QFont myFont("Monospace");
		myFont.setStyleHint(QFont::TypeWriter);
		myFont.setPointSize(myScaleFactor);
		myText->setFont(myFont);


		qreal widthOffset = myText->boundingRect().width() / 2;
		qreal heightOffset = myText->boundingRect().height() / 2;
		QPointF myPoint(myQx - widthOffset, myQy - heightOffset);
		myTextLocation = myPoint;
		myText->setPos(myPoint);

		myText->setTransformOriginPoint(myText->boundingRect().center());
		myText->setRotation(myAngle);
	}
}

bool OutputWidget::validPoint(Expression exp) {

	if (!exp.head().isList()) {
		displayText(QString("Position property is not point"));
		return false;
	}
	else if (exp.getTailLength() != 2) {
		displayText(QString("Point does not have correct number of coordinates"));
		return false;
	}
	else if(!exp.getValueInTail(0).head().isNumber() || !exp.getValueInTail(1).head().isNumber()) {
		displayText(QString("One or more coordinates are not numbers"));
		return false;
	}
	return true;

}

double OutputWidget::checkScale(Expression exp) {
	double realScaleAdjust;
	std::string scale = "\"scale\"";
	Atom myScale(scale);
	if (!exp.checkProperty(myScale)) {
		realScaleAdjust = 1;
	}
	else if (!exp.getProperty(myScale).isHeadNumber()) {
		realScaleAdjust = 1;
	}
	else if (!(exp.getProperty(myScale).head().asNumber() > 0)) {
		realScaleAdjust = 1;
	}
	else {
		realScaleAdjust = exp.getProperty(myScale).head().asNumber();
	}
	return realScaleAdjust;
}

double OutputWidget::checkRotation(Expression exp) {
	double realRotationalAdjust;
	std::string rotation = "\"rotation\"";
	Atom myRotation(rotation);
	if (!exp.checkProperty(myRotation)) {
		realRotationalAdjust = 0;
	}
	else if (!exp.getProperty(myRotation).isHeadNumber()) {
		realRotationalAdjust = 0;
	}
	else {
		realRotationalAdjust = exp.getProperty(myRotation).head().asNumber();
	}
	return realRotationalAdjust*180/(std::atan2(0, -1));
}

QGraphicsScene& OutputWidget::checkScene() {
	return *myScene;
}

QPointF OutputWidget::getLineFirstPoint() {
	return myLineFirstPoint;
}

QPointF OutputWidget::getLineSecondPoint() {
	return myLineSecondPoint;
}

qreal OutputWidget::getLineThickness() {
	return lineThickness;
}

QString OutputWidget::getLocationText() {
	return myLocationText;
}

QPointF OutputWidget::getTextLocation() {
	return myTextLocation;
}