#include <math.h>
#include <QtWidgets>

#include "edge.h"
#include "node.h"
#include "mscene.h"
#include "mgraphview.h"

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;
Edge::Edge(Node *sourceNode, Node *destNode, MScene* sc, int val)
	:source(sourceNode),dest(destNode),value(val),
	  have_one_arrow{1}, main_color(Qt::black),scene(sc)
{
	setFlag(GraphicsItemFlag::ItemIsSelectable); //возможность выделения ребра

	source->addEdge(this); //добавить у источника данное ребро и добавить
	source->addChild(dest);//к его детям вершину-ребенка

	dest->addEdge(this); //добавить к ребенку данное ребро
	setZValue(-1);
	adjust(); //пересчитать геометрию
}

int Edge::getValue() const{return value;}

//песчет геометрии
void Edge::adjust()
{
	QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
	qreal length = line.length();

	//int node_diameter = scene->m_diameter(); //извлечение диаметра из сцены
	prepareGeometryChange();

	if(length > qreal(source->diameter()/2+dest->diameter()/2-1))
	{//вершины не пересекаются
		double angle = ::acos(line.dx() / line.length());	
	{
		int node_diametr = source->diameter();
		QPointF edgeOffset((line.dx() * node_diametr/2) / length, (line.dy() * node_diametr/2) / length);
		if(source->nodeShape() == Node::Rectangle)
		{
			if(angle < 3* Pi/4 && angle > Pi/4)
				sourcePoint = line.p1() + QPointF((node_diametr/2) / tan(angle), ((line.dy()>=0)?1 : -1)*node_diametr/2);
			else
			{
				double t=angle;
				if (line.dy() >= 0)
					t = TwoPi - angle;
				sourcePoint = line.p1() + QPointF(((line.dx()>=0)?1 : -1)*node_diametr/2,-(node_diametr/2)*((line.dx()>=0)?1 : -1) *tan(t));
			}
		}
		else if(source->nodeShape() == Node::Circle)
			sourcePoint = line.p1() + edgeOffset;
		else
		{
			double t = angle;
			if (line.dx() <=0)
				t = TwoPi - angle;
			double h = (node_diametr/2)/(cos(t)/sin(t)+1);
			double w = node_diametr/2 - h;
		sourcePoint = line.p1() + QPointF(((line.dx()<=0)?-1:1)*w,
									((line.dy()<=0)?-1:1)*h);
		}

	}

	{
		int node_diametr = dest->diameter();
		QPointF edgeOffset((line.dx() * node_diametr/2) / length, (line.dy() * node_diametr/2) / length);
		if(dest->nodeShape() == Node::Rectangle)
		{
			if(angle < 3* Pi/4 && angle > Pi/4)
				destPoint = line.p2() - QPointF((node_diametr/2) / tan(angle), ((line.dy()>=0)?1 : -1)*node_diametr/2);
			else
			{
				double t = angle;
				if (line.dy() >= 0)
					t = TwoPi - angle;
				destPoint = line.p2() - QPointF(((line.dx()>=0)?1 : -1)*node_diametr/2,-(node_diametr/2)*((line.dx()>=0)?1 : -1) *tan(t));
			}
		}
		else if(dest->nodeShape() == Node::Circle)
			destPoint = line.p2() - edgeOffset;
		else
		{
			double t = angle;
			if (line.dx() <=0)
				t = TwoPi - angle;
			double h = (node_diametr/2)/(cos(t)/sin(t)+1);
			double w = node_diametr/2 - h;
	destPoint = line.p2() - QPointF(((line.dx()<=0)?-1:1)*w,
									((line.dy()<=0)?-1:1)*h);

		}

	}
	}
	else
//Если вершины заходят друг на друга
		sourcePoint = destPoint =line.p1();

	source->update();
	dest->update();
}



QRectF Edge::boundingRect() const
{
	qreal extra = (2 + scene->arrSize()) / 2.0;
	return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
									  destPoint.y() - sourcePoint.y()))
		.normalized()
		.adjusted(-extra, -extra, extra, extra);
}

//виртуальная функция рисования
void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if(sourcePoint == destPoint)//если источник и родитель в одной точке -
		return;					//выйти (при заходе вершин друг на друга)

	QLineF line(sourcePoint, destPoint);
	int arrowSize = scene->arrSize(); //считать размер стрелки

//установка ручки для рисования, в зависимости от того
//выделено ли ребро или нет
	if(this->isSelected())
		painter->setPen(QPen(main_color, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	else
		painter->setPen(QPen(main_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	painter->drawLine(line);

	double angle = ::acos(line.dx() / line.length());
	if (line.dy() >= 0)
		angle = TwoPi - angle;


	if(scene->shouldShowValues()) //если установлено свойство "отображать веса" - веса пишутся рядом
	{
		QPointF c = 0.5*line.p1() + 0.5*line.p2() +QPointF(-sin(angle)*10,cos(Pi - angle)*10) + QPointF(-15,-10);
		painter->drawText(QRectF(c,QSizeF(30,20)),Qt::AlignCenter,QString::number(value));
	}

	if(!scene->isOriented()) //если граф неориентированный - стрелки рисовать не нужно
		return;				//поэтому выход из функции


	QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 2.7) * arrowSize,
											  cos(angle - Pi / 2.7) * arrowSize);
	QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 2.7) * arrowSize,
											  cos(angle - Pi + Pi / 2.7) * arrowSize);
	painter->setBrush(main_color);
	painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);

	if(!have_one_arrow) //если у ребра не одна стрелка - отрисовать вторую
	{
		QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + Pi / 2.7) * arrowSize,
													  cos(angle + Pi / 2.7) * arrowSize);
		QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + Pi - Pi /2.7) * arrowSize,
													  cos(angle + Pi - Pi / 2.7) * arrowSize);

		painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
	}
}

//установить одну или две стрелки на ребре
void Edge::setOneArrowAtEdge(bool ori)
{
	have_one_arrow = ori;
	if(!have_one_arrow ){
//если у ребра 2 стрелки - добавить ребенку в дети родителя
		if(!dest->children().contains(source))
			dest->addChild(source);}
	else
		dest->children().removeAll(source);
	update();
}

//форма ребра - необходимая для удобного выделения
QPainterPath Edge::shape() const
{
	if(sourcePoint == destPoint)
		return QPainterPath(sourcePoint);
	QLineF line(sourcePoint, destPoint);
	double angle = ::acos(line.dx() / line.length());
	if (line.dy() >= 0)
		angle = TwoPi - angle;
	QPolygonF a;
	a<<(sourcePoint + QPointF(sin(angle + Pi / 6) * 15,cos(angle + Pi / 6) * 15))
	<<(sourcePoint + QPointF(sin(angle + Pi - Pi / 6) * 15,cos(angle + Pi - Pi / 6) * 15))
	<<(destPoint + QPointF(sin(angle - Pi + Pi / 6) * 15,cos(angle - Pi + Pi / 6) * 15))
	<<(destPoint + QPointF(sin(angle - Pi / 6) * 15, cos(angle - Pi / 6) * 15));

	QPainterPath path;
	path.addPolygon(a);
	return path;
}

//контекстное меню
void Edge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QGraphicsItem::contextMenuEvent(event);
	setSelected(true);

	QMenu menu;
		QAction * new_val = Q_NULLPTR;
		if(scene->shouldShowValues())
			 new_val = menu.addAction("Изменить вес");
	menu.addSeparator();
		QAction * add_mid_node = menu.addAction("Вставить вершину");
		QAction * remove_action = menu.addAction("Удалить ребро");

	QAction * selected_action = menu.exec(event->screenPos());

//в зависимости от действия вызвать соответствующий метод
	if(selected_action == remove_action)
		scene->removeSelectedNodes();
	else if(selected_action == add_mid_node)
		addMidNode(event->scenePos());
	else if(selected_action == new_val && scene->shouldShowValues())
	{
		bool ok;
		QString text = QInputDialog::getText(scene->getMGraph(), "Изменить вес",
													 "Вес:", QLineEdit::Normal,
													 QString::number(value),&ok);
		if(ok && !text.isEmpty())
			setValue(text.toInt());
	}

	setSelected(false);

}

void Edge::setValue(int n){value = (n>=0)?n : 0;}

void Edge::removeEdge()
{
	dest->edges().removeAll(this);
	source->edges().removeAll(this);
	source->children().removeAll(dest);

	if(!have_one_arrow) //если у ребра 2 стрелки - удалить ребенка у обоих
		dest->children().removeAll(source);

	scene->removeItem(this);//удалить со сцены
	scene->deleted << this;
}

//обработка 2-го клика - изменить вес или добавить промежуточную вершину
void Edge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseDoubleClickEvent(event);
	if(scene->shouldShowValues())
	{
		bool ok;
		QString text = QInputDialog::getText(scene->getMGraph(), "Изменить вес",
													 "Вес:", QLineEdit::Normal,
													 QString::number(value),&ok);
		if(ok && !text.isEmpty())
			setValue(text.toInt());
	} else
		addMidNode(event->scenePos());
}

//добавить промежуточную вершину
void Edge::addMidNode(QPointF pos)
{
	scene->addNode(pos);
	Node * temp_mid = scene->nodesList().last();
	scene->addEdge(source,temp_mid);
	scene->addEdge(temp_mid,dest);
	removeEdge();
	update();
}

void Edge::setColor(QColor col)
{
	if(col == Qt::white) 
		return;
	main_color = col;
	update();
}

Node * Edge::destNode() const { return dest;}

Node * Edge::sourceNode() const{ return source;}

int Edge::type() const {return Type;}
