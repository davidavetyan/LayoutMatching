#ifndef EDGE_H
#define EDGE_H
#include <QGraphicsItem>

class Node;
class MScene;
class InformationDialog;

class Edge : public QGraphicsItem
{
	friend class Node;
	friend class MScene;
	friend class InformationDialog;
public:
	Edge(Node *sourceNode, Node *destNode, MScene *sc, int val = 1);

	void setColor(QColor col);
	void removeEdge();
	void setValue(int n);

	Node* sourceNode() const;
	Node* destNode() const;
	int getValue() const;


	int type() const override;
	enum //necessary for defining the type of graphica object
	{
		Type = UserType + 2
	}; 

protected:
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	QPainterPath shape() const override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
	void adjust();//redraw when node is dragged
	void addMidNode(QPointF pos);
	void setOneArrowAtEdge(bool ori);


	Node *source, *dest;
	QPointF sourcePoint;
	QPointF destPoint;

	int value;
	bool have_one_arrow;
	QColor main_color;

	MScene* scene;
};

#endif // EDGE_H
