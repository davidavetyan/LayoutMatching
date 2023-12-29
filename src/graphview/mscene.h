#ifndef MSCENE_H
#define MSCENE_H
#include <QtWidgets>

class GraphWindow;
class MGraphView;
class Node;
class Edge;

class MScene : public QGraphicsScene
{
    friend class GraphWindow;
    friend class MGraphView;
    friend class Node;
    friend class Edge;

public:
    MScene(QObject* par = 0);
    ~MScene();

    void addNode(QPointF position, QString value = "");
    void addEdge(Node* source, Node* dest, int val = 1);

    void setDiameter(int d);
    void setOrientated(bool b);
    void setArrowSize(int d);
    void setShowEdgeValues(bool a);
    void setRectShape(int a);

    int diameter() const;
    int arrSize() const { return arrow_size; }
    bool shouldShowValues() const;
    bool isOriented() const;
    int nodeShape() const;

    QList<Node*>& nodesList();
    Node* findNode(QString val);
    Edge* findEdge(Node* source, Node* dest);
    MGraphView* getMGraph();

private:
    void setSizeOfSelectedNodes(int s);
    void setColorOfSelectedNodes(QColor c);
    void setShapeOfSelectedNodes(int shape);
    void createGraphWithText(QList<QString>& nodes, QList<QStringList>& child_of_nodes,
                             QList<QList<int>>* p_edge_weights = nullptr,
                             QList<QList<bool>>* p_special_edges = nullptr);

    QList<Node*> all_nodes;
    QList<QGraphicsItem*> deleted; //все удаленные со сцены объекты (удаляются позже)
    MGraphView* parent;

    Node* first;
    QGraphicsItem* second;

    bool show_edge_values;
    bool graph_oriented;

    int node_diameter;
    int arrow_size;
    int node_shape;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private slots:
    void connectTwoNodes();

public slots:
    void removeSelectedNodes();
};

#endif // MSCENE_H
