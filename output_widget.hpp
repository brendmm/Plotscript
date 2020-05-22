#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <complex>
#include <QWidget>
#include "interpreter.hpp"
#include "startup_config.hpp"
#include <fstream>
#include <iostream>
#include "semantic_error.hpp"
#include "qgraphicsview.h"
#include "qgraphicsscene.h"




class OutputWidget : public QWidget {
	Q_OBJECT
public:

	OutputWidget(QWidget * parent = nullptr);	

	QGraphicsScene &checkScene();
	QString getText();
	QPointF getLineFirstPoint();
	QPointF getLineSecondPoint();
	qreal getLineThickness();
	QString getLocationText();
	QPointF getTextLocation();
	void displayError(QString myString);

	public slots:
	void realChange(Expression exp);

private:

	void displayText(QString myString);
	void resizeEvent(QResizeEvent *);
	void displayPoint(Expression exp);
	void displayLine(Expression exp);
	void displayTextAtLocation(Expression exp);
	void output_Expression_as_qstring(Expression exp);
	QString output_Atom_as_qstring(Atom a);
	bool validPoint(Expression exp);
	double checkScale(Expression exp);
	double checkRotation(Expression exp);


	std::string stream;
	QGraphicsView * myView;
	QGraphicsScene * myScene;
	QString myText;
	QString myLocationText;
	QPointF myLineFirstPoint;
	QPointF myLineSecondPoint;
	qreal lineThickness;
	QPointF myTextLocation;
	Interpreter interp;

};
#endif