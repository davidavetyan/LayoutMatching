#ifndef HELPBROWSER_H
#define HELPBROWSER_H
#include <QtWidgets>
class MScene;

class InformationDialog : public QDialog
{
	Q_OBJECT

public:
	InformationDialog(const QString& strPath,const QString& strFileName, QWidget* pwgt = 0 );

	InformationDialog(MScene * scene, QWidget * par=0);

	InformationDialog(QTableWidget* table)
		:tbl(table)
	{
		setWindowFlags(Qt::CustomizeWindowHint |Qt::WindowMinMaxButtonsHint| Qt::WindowCloseButtonHint);
		setWindowModality(Qt::WindowModal);
		setWindowIcon(QIcon(":/icons/graph.png"));
		setMinimumSize(366,500);
		setWindowTitle("Алгоритм Флойда");
		QPushButton * saveButton = new QPushButton("Ок");
	//сохранение при данной кнопке
		QPushButton * rejectButton = new QPushButton("Отмена");


		connect(saveButton,SIGNAL(clicked(bool)),SLOT(accept()));
		connect(rejectButton,SIGNAL(clicked(bool)),SLOT(reject()));

		tbl->resizeColumnsToContents();
		tbl->resizeRowsToContents();

		QGridLayout * lay = new QGridLayout;
		lay->addWidget(table,0,0,1,2);
		lay->addWidget(saveButton,1,0);
		lay->addWidget(rejectButton,1,1);
		setLayout(lay);

		name = "table_view";
		readSettings();
	}

	void setTable(QTableWidget * tbl);
	void setTableItem(QTableWidgetItem * it, int column, int raw)
	{
		tbl->setItem(column,raw,it);
	}

	InformationDialog(QWidget * par=0);
	~InformationDialog(){qDebug() <<"-help";}
	static bool outDialog(const QString& text, QString title);
//функция для вызова текстового представления

private:
		InformationDialog(const QString& text_view);
//конструктор для окна текстового представления

		int new_diam; //параметры для окна настройки
		int arr_size;
		bool oriented;
		MScene* scene_for_set;
		QTableWidget * tbl;

		QString name;
		void readSettings();
		void writeSettings();
private slots:
	void set_new_diam(int a){new_diam = a;}
	void set_oriented(bool b){oriented = b;}
	void set_new_arr(int a){arr_size = a;}
	void ok_changes();
	void appl_changes();

protected:
	void closeEvent(QCloseEvent * event);
};

class FloydHelp : public QWidget
{
	Q_OBJECT
	QTableWidget * tbl;
public:
	FloydHelp(QTableWidget* table)
		:tbl(table)
	{
		setAttribute(Qt::WA_DeleteOnClose);
		setWindowFlags(Qt::CustomizeWindowHint |Qt::WindowMinMaxButtonsHint| Qt::WindowCloseButtonHint);
		setWindowModality(Qt::WindowModal);
		setWindowIcon(QIcon(":/icons/graph.png"));
		setMinimumSize(366,500);
		setWindowTitle("Алгоритм Флойда");
		QPushButton * saveButton = new QPushButton("Ок");
	//сохранение при данной кнопке
		QPushButton * rejectButton = new QPushButton("Отмена");


		tbl->resizeColumnsToContents();
		tbl->resizeRowsToContents();

		QGridLayout * lay = new QGridLayout;
		lay->addWidget(table,0,0,1,2);
		lay->addWidget(saveButton,1,0);
		lay->addWidget(rejectButton,1,1);
		setLayout(lay);

	}
	void setTableItem(QTableWidgetItem * it, int column, int raw)
	{
		tbl->setItem(column,raw,it);

	}
};


#endif // HELPBROWSER_H
