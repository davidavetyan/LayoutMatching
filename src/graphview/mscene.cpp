#include <QtWidgets>
#include <QDialog>
#include "mscene.h"
#include "node.h"
#include "edge.h"
#include "mgraphview.h"

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

MScene::MScene(QObject *par)
	: QGraphicsScene(par),
	  show_edge_values(true),
	  graph_oriented(false),
	  node_diameter(60),
	  arrow_size(15),
	  node_shape(Node::Circle)
{
	parent =  (MGraphView*)par;

	setItemIndexMethod(QGraphicsScene::NoIndex);
	connect(this, &MScene::selectionChanged, this, &MScene::connectTwoNodes);
}

int MScene::nodeShape() const
{
	return node_shape;
}

int MScene::diameter() const
{
	return node_diameter;
}

QList<Node*>& MScene::nodesList()
{
	return all_nodes;
}

MGraphView* MScene::getMGraph()
{
	return parent;
}

void MScene::setRectShape(int a)
{
	node_shape = a;
}

void MScene::setShapeOfSelectedNodes(int shape)
{
	foreach (QGraphicsItem * it, selectedItems()) {
		if(it->type() == Node::Type)
			((Node*)it)->setShape(shape);
	}
}

void MScene::setSizeOfSelectedNodes(int s)
{
	bool ok;
	int size = QInputDialog::getInt(getMGraph(), "Change Size", "Size", s, 30, 120, 1, &ok);
	if(!ok || !size)
		return;
	
	foreach (QGraphicsItem * it, selectedItems())
		if(it->type() == Node::Type)
			((Node*)it)->setDiameter(size);
}

MScene::~MScene()
{
	foreach (QGraphicsItem * it, deleted)
		delete it;

	qDebug() << "scene deleted";
}

void MScene::setDiameter(int d)
{
	node_diameter = d;
}

void MScene::setArrowSize(int d)
{
	arrow_size = d;
}

bool MScene::isOriented() const
{
	return graph_oriented;
}

void MScene::setOrientated(bool b)
{
	graph_oriented = b;
	if(b==true)
	{
		foreach (QGraphicsItem* it, items())
			if(it->type()== Edge::Type)
				((Edge*)it)->adjust();
	}
	else
	{
		foreach (QGraphicsItem* it, items())
			if(it->type()== Edge::Type)
				((Edge*)it)->setOneArrowAtEdge(b);
	}

}

void MScene::addNode(QPointF position, QString value)
{
	if(value.isEmpty())
	{
		int i = 0;
		do
			value = QString::number(i++); //определение автоматического имени
		while(findNode(value));

	}
	Node * n = new Node(this,value);
	addItem(n);
	n->setPos(position);
	all_nodes << n;

}

void MScene::addEdge(Node *source, Node *dest, int val)
{
	if(Node::is_two_nodes_connected(source,dest))
		return;

	if(graph_oriented)
	{
		if(Node::is_two_nodes_connected(dest,source))
		{
			// if connected in reverse order set two arrows and exit
			foreach(Edge * ed, dest->edges())
				if(ed->destNode() == source)
				{
					ed->setOneArrowAtEdge(false);
					return;
				}
		}
		addItem(new Edge(source, dest, this, val));
	}
	else // non oriented
	{
		if(Node::is_two_nodes_connected(dest,source) || Node::is_two_nodes_connected(source,dest))
			return;

		Edge* ed = new Edge(source,dest,this,val);
		ed->setOneArrowAtEdge(false);
		addItem(ed);
	}
}

void MScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseDoubleClickEvent(event);
	QGraphicsItem* temp = itemAt(event->scenePos().x(),event->scenePos().y(), QTransform::fromScale(1,1) );
	if(event->button() == Qt::LeftButton && temp == Q_NULLPTR )
//если клик произведен не на вершине или ребре - добавить вершину
			addNode(event->scenePos());
	qDebug() << "\n number of el " << items().size();
}

void MScene::connectTwoNodes()
{

	if(selectedItems().size() == 1 && selectedItems().last()->type() == Node::Type)
		first = (Node*)selectedItems().last();
	if(selectedItems().size() == 2  && QApplication::keyboardModifiers()== Qt::ShiftModifier)
	{
		second = (first==selectedItems().last()) ? selectedItems().first() : selectedItems().last();
		if(second->type() != Node::Type || first->type() != Node::Type)
		{
			clearSelection();
			return;
		}
		addEdge(first,(Node*)second);
		clearSelection();
		second->setSelected(true);
	}
}

void MScene::setShowEdgeValues(bool a)
{
	show_edge_values = a;
	update();
}

bool MScene::shouldShowValues() const
{
	return show_edge_values;
}

void MScene::setColorOfSelectedNodes(QColor c)
{
	QColor color = QColorDialog::getColor(c,parent,QString("Vertex color"));
	if(!color.isValid())
		return;

	foreach (QGraphicsItem* node, selectedItems())
	{
		if(node->type() == Node::Type)
			((Node*)node)->setColor(color);
	}
	clearSelection();
}

void MScene::removeSelectedNodes()
{
	foreach (QGraphicsItem* edge, selectedItems())
		if(edge->type() == Edge::Type)
			((Edge*)edge)->removeEdge();


	foreach (QGraphicsItem* node, selectedItems())
		if(node->type() == Node::Type)
			((Node*)node)->removeNode();
}

void MScene::createGraphWithText(QList<QString> &nodes, QList<QStringList> &child_of_nodes, QList<QList<int>>* p_edge_weights, QList<QList<bool>>* p_special_edges)
{
	foreach (QGraphicsItem * it, deleted) {
		delete it;
	}
	deleted.clear();

	QList<QString> all_n;
	for (int i = 0; i < nodes.size();i++)
		if(!all_n.contains(nodes[i]))
				all_n << nodes[i];
	for (int i = 0; i < nodes.size();i++)
		for(int k = 0; k < child_of_nodes[i].size();k++)
			if(!all_n.contains(child_of_nodes[i][k]))
				all_n << child_of_nodes[i][k];


	int radius = 200;
	double segments = (TwoPi)/all_n.size();
	if (all_n.size() >= 16)
		radius = all_n.size()*70/6;
	double current_angle = Pi;

	foreach(QString str,all_n)
	{
		QPointF pos(radius * cos(current_angle), radius * sin(current_angle));
		addNode(pos,str);
		current_angle += segments;
	}

	for (int i = 0; i < nodes.size(); i++)
	{
		Node * a = findNode(nodes[i]);
		for(int k = 0; k < child_of_nodes[i].size(); k++)
		{
			Node* b = findNode(child_of_nodes[i][k]);
			if (a && b)
			{
				if(a == b)
					a->setLoop();
				if (!Node::is_two_nodes_connected(a, b))
				{
					if (p_edge_weights)
						addEdge(a, b, (*p_edge_weights)[i][k]);
					else
						addEdge(a, b);
				}

				if (p_special_edges && (*p_special_edges)[i][k])
				{
					Edge* edge = findEdge(a, b);
					if (edge)
						edge->setColor(Qt::red);
				}
			}
			else
				continue;
		}
	}
}

Node* MScene::findNode(QString val)
{
	foreach (Node* node, all_nodes)
		if(node->get_val() == val)
			return node;
	return Q_NULLPTR;
}

Edge* MScene::findEdge(Node *source, Node *dest)
{
	foreach (Node* node, all_nodes)
	{
		if(node == source )
			foreach (Edge * edge, node->edges())
				if(edge->destNode()==dest)
					return edge;

		if(node == dest )
			foreach (Edge * edge, node->edges())
				if(edge->destNode()==source)
					return edge;
	}
	return Q_NULLPTR;
}

void MScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem* temp = itemAt(event->scenePos().x(),event->scenePos().y(), QTransform::fromScale(1,1) );
	if(event->modifiers() == Qt::ShiftModifier && temp)
		temp->setSelected(true);
	QGraphicsScene::mousePressEvent(event);
}


