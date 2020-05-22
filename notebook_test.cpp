#include <QTest>
#include <QSignalSpy>
#include <QGraphicsItem>
#include <cmath>

#include "notebook_app.hpp"


class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void findInputWidget();
  void findOutputWidget();
  void testKeyPressEvent();
  void outputCheckText();
  void outputCheckPoint();
  void outputCheckLine();
  void outputCheckParseError();
  void outputCheckSemanticError();

  void testDiscretePlotLayout();
  void testSimpleDiscretePlot();
  void testQuadraticDiscretePlot();
  void testContinuousPlotLayout();
  void testSimpleContinuousPlot();
  void testQuadraticContinuousPlot();
  void testSineContinuousPlot();



  // TODO: implement additional tests here
  
private:
	NotebookApp notebook;
};

void NotebookTest::initTestCase(){

}

void NotebookTest::findInputWidget() {
	auto myInput = notebook.findChild<InputWidget *>("input");
	QVERIFY2(myInput, "Could not find input widget");
}

void NotebookTest::findOutputWidget() {
	auto myOutput = notebook.findChild<OutputWidget *>("output");
	QVERIFY2(myOutput, "Could not find output widget");
}

void NotebookTest::testKeyPressEvent() {
	auto input = notebook.findChild<InputWidget *>("input");
	
	QSignalSpy checkSignal(input, SIGNAL(changed(QString)));
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);

	QCOMPARE(checkSignal.count(), 1);
	QList<QVariant> theSignal = checkSignal.takeFirst();
	QVERIFY(theSignal.at(0).type() == QVariant::String);

}

void NotebookTest::outputCheckText() {
	auto output = notebook.findChild<OutputWidget *>("output");
	auto input = notebook.findChild<InputWidget *>("input");

	input->setPlainText("(+ 1 1)");

	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	QList<QGraphicsItem *> myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 8);
	QCOMPARE(output->getText(), QString("(2)"));
	input->clear();

	input->setPlainText("(\"Hello\")");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 8);
	QCOMPARE(output->getText(), QString("(\"Hello\")"));
	input->clear();

	input->setPlainText("(define myFunction (lambda (x y) (/ x y)))");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 0);
	input->clear();

	input->setPlainText("(myFunction)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 0);
	input->clear();

	input->setPlainText("(myFunction 8 4)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 8);
	QCOMPARE(output->getText(), QString("(2)"));
	input->clear();
}

void NotebookTest::outputCheckPoint() {
	auto output = notebook.findChild<OutputWidget *>("output");
	auto input = notebook.findChild<InputWidget *>("input");

	input->setPlainText("(make-point)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);

	QList<QGraphicsItem *> myList = output->checkScene().items();

	QCOMPARE(myList.size(), 0);
	input->clear();

	input->setPlainText("(make-point 0 0)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 4);
	QCOMPARE(myList[0]->sceneBoundingRect(),QRectF(0,0,0,0));
	input->clear();

	input->setPlainText("(make-point 9 15)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 4);
	QCOMPARE(myList[0]->sceneBoundingRect(), QRectF(9, 15, 0, 0));
	input->clear();

	input->setPlainText("(set-property \"size\" 50 (make-point 0 0))");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 4);
	QCOMPARE(myList[0]->sceneBoundingRect(), QRectF(-25, -25, 50, 50));
	input->clear();

	input->setPlainText("(set-property \"size\" 50 (make-point 9 15))");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 4);
	QCOMPARE(myList[0]->sceneBoundingRect(), QRectF(-16, -10, 50, 50));
	input->clear();

	input->setPlainText("(begin (define xloc 0) (define yloc 0) (list (set-property \"size\" 2 (make-point 0 4)) (set-property \"size\" 4 (make-point 0 8)) (set-property \"size\" 8 (make-point 0 16)) (set-property \"size\" 16 (make-point 0 32)) (set-property \"size\" 32 (make-point 0 64))))");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(1000);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 5);
	int x;
	for (int i = 0; i < myList.size(); i++) {
		QCOMPARE(myList[i]->type(), 4);
		x = myList.size() - i;
		QCOMPARE(myList[i]->sceneBoundingRect(), QRectF((0 - (pow(2, x) / 2)), (pow(2,x+1) - (pow(2, x) / 2)), (pow(2, x)), (pow(2, x))));
	}
	input->clear();
}

void  NotebookTest::outputCheckLine() {
	auto output = notebook.findChild<OutputWidget *>("output");
	auto input = notebook.findChild<InputWidget *>("input");

	input->setPlainText("(make-line)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);

	QList<QGraphicsItem *> myList = output->checkScene().items();

	QCOMPARE(myList.size(), 0);
	input->clear();

	input->setPlainText("(make-line (make-point 0 0) (make-point 50 50))");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 6);
	QCOMPARE(output->getLineFirstPoint(), QPointF(0, 0));
	QCOMPARE(output->getLineSecondPoint(), QPointF(50, 50));
	QCOMPARE(output->getLineThickness(), qreal(1));
	input->clear();

	input->setPlainText("(set-property \"thickness\" 7 (make-line (make-point -7 9) (make-point 32 -93)))");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(myList[0]->type(), 6);
	QCOMPARE(output->getLineFirstPoint(), QPointF(-7, 9));
	QCOMPARE(output->getLineSecondPoint(), QPointF(32 ,-93));
	QCOMPARE(output->getLineThickness(), qreal(7));
	input->clear();


}

void NotebookTest::outputCheckParseError() {
	auto output = notebook.findChild<OutputWidget *>("output");
	auto input = notebook.findChild<InputWidget *>("input");

	input->setPlainText("(begin))");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);

	QList<QGraphicsItem *> myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(output->getText(), QString("Error: Invalid Expression. Could not parse."));
	input->clear();
}

void NotebookTest::outputCheckSemanticError() {
	auto output = notebook.findChild<OutputWidget *>("output");
	auto input = notebook.findChild<InputWidget *>("input");

	input->setPlainText("(a)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);

	QList<QGraphicsItem *> myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(output->getText(), QString("Error during evaluation: unknown symbol"));
	input->clear();

	input->setPlainText("(- 9 7 6)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(output->getText(), QString("Error in call to subtraction or negation: invalid number of arguments."));
	input->clear();

	input->setPlainText("(first 1)");
	QTest::keyPress(input, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(100);
	myList = output->checkScene().items();

	QCOMPARE(myList.size(), 1);
	QCOMPARE(output->getText(), QString("Error in call to first: argument is not a list."));
	input->clear();
}

/*
------------------------------------PLOT TESTS---------------------------------------
*/

/*
findLines - find lines in a scene contained within a bounding box
with a small margin
*/
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin) {

	QPainterPath selectPath;

	QMarginsF margins(margin, margin, margin, margin);
	selectPath.addRect(bbox.marginsAdded(margins));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);
	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
*/
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numpoints(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsEllipseItem::Type) {
			numpoints += 1;
		}
	}

	return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
rotation and string contents
*/
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents) {
	int numtext(0);
	foreach(auto item, scene->items(center)) {
		if (item->type() == QGraphicsTextItem::Type) {
			QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
			if ((text->toPlainText() == contents) &&
				(text->rotation() == rotation) &&
				(text->pos() + text->boundingRect().center() == center) &&
				1) {
					numtext += 1;
			}
		}
	}

	return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
*/
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius) {
	
	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

void NotebookTest::testDiscretePlotLayout() {

	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	auto inputWidget = notebook.findChild<InputWidget *>("input");

	std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1) ) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(5000);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	QCOMPARE(items.size(), 17);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 2.0;
	double scaley = 20.0 / 2.0;

	double xmin = scalex*-1;
	double xmax = scalex * 1;
	double ymin = scaley*-1;
	double ymax = scaley * 1;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	// check the point at (-1,-1)
	QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

	// check the point at (1,1)
	QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}

void NotebookTest::testSimpleDiscretePlot() {

	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	auto inputWidget = notebook.findChild<InputWidget *>("input");

	std::string program = R"(
	(begin
    (define f (lambda (x) 
        (list x (+ (* 2 x) 1))))
    (discrete-plot (map f (range -2 2 0.5))
       (list
       (list "title" "The Data")
       (list "abscissa-label" "X Label")
       (list "ordinate-label" "Y Label")
       (list "text-scale" 1))))
	)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(5000);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 15 lines + 9 points + 7 text = 31
	auto items = scene->items();
	QCOMPARE(items.size(), 31);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 4.0;
	double scaley = 20.0 / 8.0;

	double xmin = scalex*-2;
	double xmax = scalex * 2;
	double ymin = scaley*-3;
	double ymax = scaley * 5;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	//check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Data")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-2")), 1);
	
	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("2")), 1);
	
	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-3")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("5")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 2);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 2);

	// check the point at (-1,-1)

	for (double i = 0; i < 9; i++) {
		QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * i) , -ymin - (2.5 * i) ), 0.6), 1);
	}
	
}

void NotebookTest::testQuadraticDiscretePlot() {

	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	auto inputWidget = notebook.findChild<InputWidget *>("input");

	std::string program = R"(
	(begin
    (define f (lambda (x) 
        (list x (+ (* x x) 1))))
    (discrete-plot (map f (range -2 2 0.5))
       (list
       (list "title" "The Data")
       (list "abscissa-label" "X Label")
       (list "ordinate-label" "Y Label")
       (list "text-scale" 1))))
	)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(5000);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 15 lines + 9 points + 7 text = 31
	auto items = scene->items();
	QCOMPARE(items.size(), 30);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 4.0;
	double scaley = 20.0 / 4.0;

	double xmin = scalex*-2;
	double xmax = scalex * 2;
	double ymin = scaley*1;
	double ymax = scaley * 5;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	//check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Data")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-2")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("2")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("5")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 2);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	//QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 2);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 2);

	// check the point at (-1,-1)


	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 0), -ymax), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 1), -16.25), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 2), -10), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 3), -6.25), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 4), -5), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 5), -6.25), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 6), -10), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 7), -16.25), 0.6), 1);
	QCOMPARE(findPoints(scene, QPointF(xmin + (2.5 * 8), -ymax), 0.6), 1);


}

void NotebookTest::testContinuousPlotLayout() {

	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	auto inputWidget = notebook.findChild<InputWidget *>("input");

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (x)))
    (continuous-plot f (list -2 2)
	    (list
    (list "title" "A continuous linear function")
    (list "abscissa-label" "x")
    (list "ordinate-label" "y"))))
	)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(5000);
	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 6 grid lines + 50 plot lines + 7 text = 63
	auto items = scene->items();
	QCOMPARE(items.size(), 63);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 4.0;
	double scaley = 20.0 / 4.0;

	double xmin = scalex*-2;
	double xmax = scalex * 2;
	double ymin = scaley*-2;
	double ymax = scaley * 2;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("A continuous linear function")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("x")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("y")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-2")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("2")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-2")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("2")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 1);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 1);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

}

void NotebookTest::testSimpleContinuousPlot() {

	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	auto inputWidget = notebook.findChild<InputWidget *>("input");

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (+ (* 2 x) 1)))
    (continuous-plot f (list -2 2)
	    (list
    (list "title" "A continuous linear function")
    (list "abscissa-label" "x")
    (list "ordinate-label" "y"))))
	)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(5000);
	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 6 grid lines + 50 plot lines + 7 text = 63
	auto items = scene->items();
	QCOMPARE(items.size(), 63);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 4.0;
	double scaley = 20.0 / 8.0;

	double xmin = scalex*-2;
	double xmax = scalex * 2;
	double ymin = scaley*-3;
	double ymax = scaley * 5;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("A continuous linear function")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("x")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("y")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-2")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("2")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-3")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("5")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 1);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 1);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	//QCOMPARE(intersectsLine(scene, QPointF(xmiddle,ymiddle), 10.1), 56);

}

void NotebookTest::testQuadraticContinuousPlot() {

	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	auto inputWidget = notebook.findChild<InputWidget *>("input");

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (+ (* x x) 1)))
    (continuous-plot f (list -2 2)
	    (list
    (list "title" "A continuous linear function")
    (list "abscissa-label" "x")
    (list "ordinate-label" "y"))))
	)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(5000);
	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 6 grid lines + 50 plot lines + 7 text = 63
	auto items = scene->items();
	QCOMPARE(items.size(), 80);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 4.0;
	double scaley = 20.0 / 4.0;

	double xmin = scalex*-2;
	double xmax = scalex * 2;
	double ymin = scaley*1;
	double ymax = scaley * 5;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("A continuous linear function")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("x")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("y")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-2")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("2")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("5")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 11);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 1);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	//QCOMPARE(intersectsLine(scene, QPointF(xmiddle,ymiddle), 10.1), 56);

}

void NotebookTest::testSineContinuousPlot() {

	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	auto inputWidget = notebook.findChild<InputWidget *>("input");

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (+ (sin x) 0)))
    (continuous-plot f (list (- pi) pi)
	    (list
    (list "title" "A continuous linear function")
    (list "abscissa-label" "x")
    (list "ordinate-label" "y"))))
	)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
	QTest::qWait(5000);
	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 6 grid lines + 74 plot lines + 7 text = 63
	auto items = scene->items();
	QCOMPARE(items.size(), 87);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}


	double xmin = -10;
	double xmax = 10;
	double ymin = -10;
	double ymax = 10;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("A continuous linear function")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("x")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("y")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-3.1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("3.1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 7);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 7);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 1);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	//QCOMPARE(intersectsLine(scene, QPointF(xmiddle,ymiddle), 10.1), 56);

}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
