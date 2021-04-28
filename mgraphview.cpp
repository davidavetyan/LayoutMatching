#include <math.h>
#include "node.h"
#include "mgraphview.h"
#include "mscene.h"
#include "graphwindow.h"
MGraphView::MGraphView(QWidget *parent)
	: QGraphicsView(parent)
{
	scene = new MScene(this);
	setScene(scene);

	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setOptimizationFlags(DontSavePainterState | DontClipPainter | DontAdjustForAntialiasing);
	setRenderHint(QPainter::Antialiasing, true);

	scale(qreal(1), qreal(1));
	setWindowIcon(QIcon(":/icons/graph.png"));
	setAcceptDrops(true);
	setDragMode(QGraphicsView::RubberBandDrag);
	setTransformationAnchor(AnchorUnderMouse);


}
MGraphView::~MGraphView()
{
	delete scene;
}

//обработка событий клавиш
void MGraphView::keyPressEvent(QKeyEvent *event)
{
	if(scene->selectedItems().isEmpty())
		return;
	switch (event->key()) {
	case Qt::Key_Delete: case Qt::Key_X:
			scene->removeSelectedNodes();
		break;
	case Qt::Key_C:
			scene->setColorOfSelectedNodes(Qt::white);
		break;
	case Qt::Key_S:
		scene->setShapeOfSelectedNodes(Node::Rectangle);
		break;
	case Qt::Key_A:
		scene->setShapeOfSelectedNodes(Node::Circle);
		break;
	case Qt::Key_Z:
		scene->setShapeOfSelectedNodes(Node::Rhomb);
		break;
	case Qt::Key_D:
		scene->setSizeOfSelectedNodes(60);
	default:
		break;
	}
		QGraphicsView::keyPressEvent(event);
}

//обработка колеса мыши
void MGraphView::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, event->delta() / 240.0));

}

//масштабирование
void MGraphView::scaleView(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}

//перетаскивание файла в виджет
void MGraphView::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasText())
		event->accept();
	else
		event->ignore();
}

//открытие графа при перетаскивании
void MGraphView::dropEvent(QDropEvent *event)
{
	qDebug() <<"oop!\n";
		QString str = event->mimeData()->text().remove("file:///");

		GraphWindow * p = dynamic_cast<GraphWindow *>(parent());

		if (str.indexOf(".grph") != -1)
		{
			p->createNewGraphWithFormat(str);
		}
		else if (str.indexOf(".txt") != -1)
		{
			QFile File(str);

			if(File.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				p->createNewGraph(File.readAll());
				File.close();
			}
		}
}

void MGraphView::dragMoveEvent(QDragMoveEvent *event){Q_UNUSED(event)}

void MGraphView::dragLeaveEvent(QDragLeaveEvent *event){Q_UNUSED(event);}
