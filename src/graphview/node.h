#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>


class Edge;
class MGraphView;
class MScene;

/**
    \brief Represents a graph node
*/
class Node : public QGraphicsItem
{
    friend class Edge;
    friend class MScene;
    friend QString& operator<<(QString& str, const Node& node);
    
public:
    Node(MScene* graphWidget, QString val = "");

    void setColor(QColor col);
    void setValue(QString val);
    void removeNode();
    void setLoop();
    void removeLoop();
    void setShape(int shape);
    void setDiameter(int diam);

    QString get_val() const;
    QColor get_color() const;

    QList<Edge*>& edges();
    QList<Node*>& children();

    static bool are_nodes_connected(Node* src, Node* dst);

    enum
    {
        Type = UserType + 1
    };

    int nodeShape() const;
    int diameter() const;
    int type() const override;
    enum
    {
        Circle = 0,
        Rhomb,
        Rectangle
    };

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void addChild(Node* child);
    void addEdge(Edge* edge);

private:
    QList<Edge*> edgeList;
    QList<Node*> childrenList;

    MScene* scene; // scene where the vertex is located

    QColor main_color;
    QString value;
    int shape_type;
    int m_diameter;
};

#endif // NODE_H
