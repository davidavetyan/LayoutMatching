#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include "edge.h"
#include "helpbrowser.h"
#include "mscene.h"
#include "node.h"
#include <QWidget>
#include <QtWidgets>

class MScene;


struct tbl_wid
{
    int column, raw;
    QString value;
    tbl_wid(int a, int b, QString c) : column(a), raw(b), value(c) {}
};

namespace Ui {
class GraphWindow;
}

class GraphWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWindow(QWidget* parent = 0);
    ~GraphWindow();

    MScene* getMscene();

    bool createNewGraph(QString temp);                //create graph from string temp
    bool createNewGraphWithFormat(QString file_name); //create formatted graph from file_name
    void createGraphFromMatrix(std::vector<std::vector<int>> const& adjMatrix,
                               std::vector<std::pair<int, int>>* pSpecialEdges = nullptr);

    void writePathSettings(QString str);
    QString readPathSettings();

protected:
    void closeEvent(QCloseEvent* event);

private:
    QString add_spaces_and_simplifie(QString str_for_work);

    void setFormat();
    void clearFormat();

    void writeSettings();
    void readSettings();

    void generate(int k, QString& res);
    QVector<QString> m_nodes;
    bool gamPyt();
    int size;

    FloydHelp* floyd_brows;

private slots:
    void savePicture();
    void saveTextGraph();
    void saveGraphWithFormat();

    void cleanGraph();
    void selectAll();

    void showValues(bool a); //show edge values
    void setDelay(int a);

    void advSet(); //show advanced settings
    void showCreateHints();
    void showTextGraph();
    void setRectShape();
    void changeToolBut(int a);

    void wide_search();
    void deep_search();
    void floyd_alg();
    void get_tbl_wid(QString str);
    void floyd_final();

    void hamilton_alg();

private:
    QList<QString> nodes;
    QList<QStringList> child_of_nodes;
    QList<QPointF> points_of_nodes;
    QList<QColor> color_of_nodes;
    QList<int> shapes_of_nodes;
    QList<int> diametrs_of_nodes;

    Ui::GraphWindow* ui;

    bool processing; //is any algorithm in process
    int msec_delay;  //delay between algorithm steps
};


void set_delay(int msec);


class FloydCalc : public QObject
{
    Q_OBJECT

private:
    int size;
    QTableWidget* tbl;
    MScene* scene;
    InformationDialog* help;

signals:
    void finished();
    void setInt(int a);

    void sendInfo(QString a);

public:
    FloydCalc(MScene* sc) : scene(sc)
    {
        size = scene->nodesList().size();
        //connect(this,&FloydCalc::finished,this,&FloydCalc::showHelp);
    }

public slots:
    QTableWidget* getWindow() { return tbl; }

    void showHelp() { emit finished(); }


    void calculation()
    {
        int** matrix;

        int INF = 1000000;
        matrix = new int*[size];

        for (int i = 0; i < size; i++)
            matrix[i] = new int[size];

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                matrix[i][j] = INF;
            matrix[i][i] = 0;
        }

        foreach(Node* node, scene->nodesList())
        {
            int a = scene->nodesList().indexOf(node);
            foreach(Node* ch, node->children())
            {
                int b = scene->nodesList().indexOf(ch);
                Edge* e = scene->findEdge(node, ch);
                matrix[a][b] = e->getValue();
            }
        }

        for (int k = 0; k < size; ++k)
        {
            for (int i = 0; i < size; ++i)
            {

                for (int j = 0; j < size; ++j)
                {
                    matrix[i][j] = qMin(matrix[i][j], matrix[i][k] + matrix[k][j]);
                }
            }
        }


        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                QString str((matrix[i][j] != INF) ? QString::number(matrix[i][j]) : "∞");

                QString a(QString::number(i) + "|" + QString::number(j) + "|" + str);

                emit sendInfo(a);
            }
        }

        for (int i = 0; i < size; i++)
            delete[] matrix[i];
        delete[] matrix;
        emit finished();
    }
};

#endif // GRAPHWINDOW_H
