#include "graphwindow.h"
#include "ui_graphwindow.h"

#include "mscene.h"
#include "node.h"
#include "edge.h"
#include "helpbrowser.h"



GraphWindow::GraphWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GraphWindow)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);

	// Create menu bar
//	QMenu * file =  new QMenu("Файл");
//		file->addAction("Сохранить как текст (.txt)", this, SLOT(saveTextGraph()),QKeySequence("Ctrl+S"));
//		file->addAction("Сохранить как форматированный граф (.grph)",this,SLOT(saveGraphWithFormat()),QKeySequence("Ctrl+Shift+S"));
//		file->addSeparator();
//		file->addAction("Сохранить как картинку (.png)",this,SLOT(savePicture()),QKeySequence("Ctrl+P"));
//
//	QMenu * graph = new QMenu("Граф");
//		graph->addAction("Текстовое представление",this, SLOT(showTextGraph()));
//	//	graph->addAction("Таблица смежности");
//		graph->addSeparator();
//			QAction * show_values = new QAction("Отображать веса");
//			show_values->setShortcut(QKeySequence("Ctrl+D"));
//			show_values->setCheckable(true);
//			connect(show_values,SIGNAL(triggered(bool)),this,SLOT(showValues(bool)));
//		graph->addAction(show_values);
//		graph->addSeparator();
//		graph->addAction("Расширенные настройки",this,SLOT(advSet()),QKeySequence("Ctrl+T"));
//		graph->addAction("Выделить все", this, SLOT(selectAll()),QKeySequence("Ctrl+A"));
//		graph->addAction("Очистить",this, SLOT(cleanGraph()));
//
//	QMenu * info = new QMenu("Справка");
//		info->addAction("Создание графа",this,SLOT(showCreateHints()));
//
//	QMenu * alg = new QMenu("Алгоритмы");
////		alg->addAction("Поиск в ширину",this, SLOT(wide_search()));
////		alg->addAction("Поиск в глубину",this,SLOT(deep_search()));
//		alg->addSeparator();
//		alg->addAction("Алгоритм Флойда",this,SLOT(floyd_alg()));
//		//alg->addAction("Алгоритм Гамильтона",this, SLOT(hamilton_alg()));
//	QMenuBar * menuBar = new QMenuBar(this);
//	menuBar->addMenu(file);
//	menuBar->addMenu(graph);
//	menuBar->addMenu(info);
//	menuBar->addMenu(alg);
//	ui->gridLayout->setMenuBar(menuBar);
	ui->main_graph->setParent(this);


	setWindowTitle("Graph viewer");
	setWindowIcon(QIcon(":/icons/graph.png"));
	processing = false;

	//connect(ui->save_as_grph,SIGNAL(clicked(bool)),this,SLOT(saveGraphWithFormat()));
	//connect(ui->save_as_txt,SIGNAL(clicked(bool)),this,SLOT(saveTextGraph()));
	//connect(ui->save_as_png,SIGNAL(clicked(bool)),this,SLOT(savePicture()));
	
	connect(ui->set_tool,SIGNAL(clicked(bool)),this,SLOT(advSet()));
	connect(ui->show_val,SIGNAL(toggled(bool)),this,SLOT(showValues(bool)));
	connect(ui->clear_all,&QPushButton::clicked,getMscene(),&MScene::removeSelectedNodes);
	//connect(ui->faq,SIGNAL(clicked(bool)),this,SLOT(showCreateHints()));

	connect(ui->circle,&QRadioButton::toggled,this,&GraphWindow::setRectShape);
	connect(ui->rect,&QRadioButton::toggled,this,&GraphWindow::setRectShape);
	connect(ui->rhomb,&QRadioButton::toggled,this,&GraphWindow::setRectShape);

	QSlider * diam_slider = new QSlider(Qt::Horizontal);
	ui->toolButton->setText("60");
	diam_slider->setRange(30,120);
	diam_slider->setFixedWidth(200);
	diam_slider->setValue(getMscene()->diameter());
	diam_slider->setSingleStep(1);
	diam_slider->setPageStep(5);
	diam_slider->setTickInterval(5);
	QHBoxLayout * lay = new QHBoxLayout;
	lay->addWidget(diam_slider);

	QWidgetAction * act = new QWidgetAction(ui->toolButton);
	act->setDefaultWidget(diam_slider);

	QMenu * sl_menu = new QMenu(ui->toolButton);
	sl_menu->addAction(act);
	ui->toolButton->setMenu(sl_menu);

	connect(diam_slider,&QSlider::valueChanged,this, &GraphWindow::changeToolBut);
	msec_delay = 50;
	readSettings();
}

void GraphWindow::changeToolBut(int a)
{
	ui->toolButton->setText(QString::number(a));
	getMscene()->setDiameter(a);
}

GraphWindow::~GraphWindow()
{
	delete ui;
	qDebug() << "\nwindow deleted";
}

void GraphWindow::showValues(bool a)
{
	getMscene()->setShowEdgeValues(a);
}

void GraphWindow::advSet()
{
	InformationDialog brows(getMscene(),this);
	brows.exec();
}

void GraphWindow::setRectShape()
{
	if(ui->circle->isChecked())
		getMscene()->setRectShape(Node::Circle);
	else if(ui->rect->isChecked())
		getMscene()->setRectShape(Node::Rectangle);
	else if(ui->rhomb->isChecked())
		getMscene()->setRectShape(Node::Rhomb);
}

void GraphWindow::selectAll()
{
	foreach (QGraphicsItem * node, getMscene()->nodesList())
		node->setSelected(true);
}

void GraphWindow::savePicture()
{
	if(getMscene()->nodesList().isEmpty())
		return;
	QString FileName = QFileDialog::getSaveFileName(this, tr("Save file"),readPathSettings(),
													tr("Image (*.png)"));
	if(FileName.isEmpty())
		return;
	if(ui->main_graph->getMscene()->nodesList().isEmpty())
		return;

	writePathSettings(FileName);

	QImage image(ui->main_graph->sceneRect().size().toSize(),QImage::Format_ARGB32);
	image.fill(Qt::transparent);
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	ui->main_graph->getMscene()->render(&painter);
	image.save(FileName);
}

void GraphWindow::showTextGraph()
{
	QString a;
	foreach (Node* node, ui->main_graph->getMscene()->nodesList())
	{
		a << *node;
		a.append("\n");
	}

	if(InformationDialog::outDialog(a,"Graph representation in text"))
		saveTextGraph();
}

void GraphWindow::saveTextGraph()
{
	QString a;
	foreach (Node* node, ui->main_graph->getMscene()->nodesList())
	{
		a << *node;
		a.append("\n");
	}

	if(a.size() == 0)
		return;

	QString FileName = QFileDialog::getSaveFileName(this, tr("Save file"),readPathSettings(),
													tr("text(*.txt)"));
	if (FileName.isEmpty())
		return;

	writePathSettings(FileName);

	QFile File(FileName);
	if(File.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QByteArray Data;
		Data.append(a);
		File.write(Data);
		File.close();
	}
}

void GraphWindow::saveGraphWithFormat()
{
	QString a;
	if(getMscene()->isOriented())
		a.append("oriented\n");
	else
		a.append("nonoriented\n");

	foreach (Node* node, ui->main_graph->getMscene()->nodesList())
	{
		a << *node;
		a.append(" &%& " + QString::number(node->scenePos().x()) + " " + QString::number(node->scenePos().y())

				 +" &%& " + QString::number(node->get_color().red()) +
				 " " + QString::number(node->get_color().green()) +
				 " " + QString::number(node->get_color().blue())

				 +" &%& " + QString::number(node->nodeShape() )
				 + " &%& " + QString::number(node->diameter())+ "\n");
	}

	if(a.size() == 0)
		return;

	QString FileName = QFileDialog::getSaveFileName(this, tr("Save file"),readPathSettings(),
													tr("Graph File(*.grph)"));
	if (FileName.isEmpty())
		return;

	writePathSettings(FileName);

	QFile File(FileName);
	if(File.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QByteArray Data;
		Data.append(a);
		QByteArray d = Data.toBase64();
		File.write(Data);
		File.close();
	}
}

MScene* GraphWindow::getMscene()
{
	return ui->main_graph->getMscene();
}

void GraphWindow::cleanGraph()
{
	foreach (Node* node, getMscene()->nodesList())
		node->removeNode();

}

bool GraphWindow::createNewGraphWithFormat(QString file_name)
{
	color_of_nodes.clear();
	shapes_of_nodes.clear();
	points_of_nodes.clear();
	diametrs_of_nodes.clear();
	QString temp;
	QFile File(file_name);
	if(File.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QByteArray arr;
		temp = File.readAll();
		//temp = QByteArray::fromBase64(arr);
		File.close();
	}
	 else
		return false;

	QString for_next;

	QTextStream stream(&temp);

	QString orientation = stream.readLine().trimmed();
	qreal a,b, c;
	while(!stream.atEnd())
	{
		QString temp2 = stream.readLine(120);
		if (temp2.isEmpty())
			continue;
		QStringList list1 = temp2.split("&%&");
		for_next.append(list1.first().trimmed() + "\n");

		QTextStream temp3(&list1[1]);
		temp3 >> a >> b;
		points_of_nodes << QPointF(a,b);

		temp3.setString(&list1[2]);
		temp3 >> a >> b >> c;
		color_of_nodes << QColor(a,b,c);

		shapes_of_nodes << list1[3].toInt();
		diametrs_of_nodes << list1[4].toInt();
	}
	if(!createNewGraph(for_next))
		return false;
	setFormat();
	if(orientation == "nonoriented")
		getMscene()->setOrientated(false);
	else
		getMscene()->setOrientated(true);
	return true;

}

void GraphWindow::createGraphFromMatrix(std::vector<std::vector<int>> const& adjMatrix,
	std::vector<std::pair<int, int>>* pSpecialEdges)
{
	nodes.clear();
	child_of_nodes.clear();
	cleanGraph();

	QList<QList<int>> edge_weights;
	QList<QList<bool>> special_edges;

	QString sPrefix = "R";
	for (int i = 0; i < adjMatrix.size(); i++)
	{
		QString t = sPrefix + QString::number(i);
		nodes << t;

		QStringList list_of_children;
		QList<int> list_of_edge_weights;
		QList<bool> list_of_special_edges;
		for (int j = 0; j < adjMatrix[i].size(); j++)
		{
			int edge_weight = adjMatrix[i][j];
			if (edge_weight)
			{
				list_of_children << sPrefix + QString::number(j);
				list_of_edge_weights << edge_weight;

				if (pSpecialEdges)
				{
					auto it = std::find((*pSpecialEdges).begin(), (*pSpecialEdges).end(), std::pair<int, int>(i, j));
					if (it != (*pSpecialEdges).end())
						list_of_special_edges << true;
					else
						list_of_special_edges << false;
				}
			}
		}

		child_of_nodes << list_of_children;
		edge_weights << list_of_edge_weights;
		special_edges << list_of_special_edges;
	}
	
	ui->main_graph->getMscene()->createGraphWithText(nodes, child_of_nodes, &edge_weights, &special_edges);
}

void GraphWindow::setFormat()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		Node * node = getMscene()->nodesList().at(i);
		node->setColor(color_of_nodes[i]);
		node->setPos(points_of_nodes[i]);
		node->setShape(shapes_of_nodes[i]);
		node->setDiameter(diametrs_of_nodes[i]);
		node->update();
	}
}

bool GraphWindow::createNewGraph(QString temp)
{
	nodes.clear();
	child_of_nodes.clear();
	cleanGraph();
	temp  = temp.trimmed();
	QTextStream stream(&temp);
	while(!stream.atEnd())
	{
		QString line = add_spaces_and_simplifie(stream.readLine(120));
		if(line.isEmpty()) continue;

		QStringList list = line.split(":");
		if(list.first() == list.last()) return false;

		QString t = list.first().trimmed();
		nodes << t;

		QString t2 = list.last().trimmed();
		QStringList list_of_children = t2.split(" , ",QString::SkipEmptyParts);

		child_of_nodes << list_of_children;
	}
	ui->main_graph->getMscene()->createGraphWithText(nodes,child_of_nodes);
	return true;
}

QString GraphWindow::add_spaces_and_simplifie(QString str_for_work)
{
	QChar symbs[2]{':',','};
	for(int i = 0; i < 2; i++)
	{
		int a = -2;
		while((a = str_for_work.indexOf(symbs[i],a+2))!=-1)
		{
			str_for_work.insert(a, ' ');
			str_for_work.insert(a+2, ' ');
		}
	}
	str_for_work = str_for_work.simplified();
	return str_for_work;
}

void GraphWindow::showCreateHints()
{
	InformationDialog brows(":/help","graph_creation.htm");
	brows.exec();
}

void set_delay(int msec)
{
	QTime dieTime= QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void GraphWindow::setDelay(int a){	msec_delay = a;}

void GraphWindow::clearFormat()
{
	ui->gridLayout->menuBar()->setEnabled(true);
	//processing = false;
	foreach (QGraphicsItem * it, getMscene()->items()) {
			if(it->type() == Node::Type)
				((Node*)it)->setColor(Qt::white);

			if(it->type() == Edge::Type)
				((Edge*)it)->setColor(Qt::black);
	}
		getMscene()->getMGraph()->setDisabled(false);
}

void GraphWindow::wide_search()
{
	QList<Node*> list = getMscene()->nodesList();
	if(list.isEmpty())
		return;

	if(getMscene()->selectedItems().size() != 1 ||
			getMscene()->selectedItems().first()->type() == Edge::Type)
		return;

	bool ok;
	QString text = QInputDialog::getText(getMscene()->getMGraph(), "Search By Vertex",
												 "Vertex name:", QLineEdit::Normal,
												 "for searching", &ok);
	if(!ok)
		return;
	text = text.trimmed();

	processing = 1;
	msec_delay = 50;

	QSlider * slider = new QSlider(Qt::Horizontal);
	slider->setRange(1,300);
	slider->setValue(50);
	slider->setSingleStep(1);
	slider->setPageStep(1);
	slider->setTickInterval(50);
	slider->setMinimumWidth(160);
	slider->setTickPosition(QSlider::TicksBelow);
	slider->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
	slider->setWindowModality(Qt::WindowModal);
	slider->setWindowTitle("Задержка алгоритма");
	slider->move(this->pos());
	slider->show();
	connect(slider,&QSlider::valueChanged,this,&GraphWindow::setDelay);

	ui->gridLayout->menuBar()->setDisabled(true);

	QVector<bool> is_visited;
	is_visited.resize(list.size());
	QQueue<Node*> queue;
	Node* node = (Node*)getMscene()->selectedItems().first();
	is_visited[list.indexOf(node)] = true;;

	queue.push_front(node);
	node->setColor(Qt::red);

	while(!queue.isEmpty() && processing)
	{
		Node * n = queue.first();
		queue.pop_front();

		if(n->get_val() == text)
		{
			QMessageBox::information(this, " ", "Vertex " + text + " not found", QDialogButtonBox::Ok);
			slider->close();
			clearFormat();
			n->setColor(Qt::blue);
			return;
		}

		foreach(Node* nid, n->children())
		{
			if(!processing)
				break;
			if(is_visited[list.indexOf(nid)])
				continue;

			queue.push_back(nid);

			foreach(Edge * e, n->edges())
			{
				if(e->destNode() == nid || e->sourceNode()  == nid)
					e->setColor(Qt::red);
			}

			set_delay(msec_delay);
			nid->setColor(Qt::red);

			is_visited[list.indexOf(nid)] = true;
		}
	}
	
	QMessageBox::information(this," ","Vertex not found!", QDialogButtonBox::Ok);
	slider->close();
	clearFormat();
}

void GraphWindow::deep_search()
{
	QList<Node*> list = getMscene()->nodesList();
	if(list.isEmpty())
		return;
	if(	getMscene()->selectedItems().size() != 1 ||
		getMscene()->selectedItems().first()->type() == Edge::Type)
		return;
	
	bool ok;
	QString text = QInputDialog::getText(getMscene()->getMGraph(), "Search By Vertex",
												 "Vertex name:", QLineEdit::Normal,
												 "for searching",&ok);
	if(!ok)
		return;
	text = text.trimmed();

	processing = 1;
	msec_delay = 50;
	QSlider * slider = new QSlider(Qt::Horizontal);
	slider->setRange(1,300);
	slider->setValue(50);
	slider->setSingleStep(1);
	slider->setPageStep(1);
	slider->setTickInterval(50);
	slider->setMinimumWidth(160);
	slider->setTickPosition(QSlider::TicksBelow);
	slider->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
	slider->setWindowModality(Qt::WindowModal);
	slider->setWindowTitle("Algorithm delay");
	slider->move(this->pos());
	slider->show();
	connect(slider,&QSlider::valueChanged,this,&GraphWindow::setDelay);
	ui->gridLayout->menuBar()->setDisabled(true);

//алгоритм
	QVector<bool> is_visited;
	is_visited.resize(list.size());
	QQueue<Node*> queue;
	Node* node = (Node*)getMscene()->selectedItems().first();
	is_visited[list.indexOf(node)] = true;;

	queue.push_back(node);
	node->setColor(Qt::red);

	while(!queue.isEmpty() && processing)

	{
		Node * n = queue.last();
		queue.pop_back();

		if(n->get_val() == text)
		{
			QMessageBox::information(this," ","Vertex " + text + " found",QDialogButtonBox::Ok);
			slider->close();
			clearFormat();
			n->setColor(Qt::blue);
			return;
		}


		foreach(Node* nid, n->children())
		{
			if(!processing)
				break;
			if(is_visited[list.indexOf(nid)])
				continue;

			queue.push_back(nid);

			foreach(Edge * e, n->edges())
				if(e->destNode() == nid || e->sourceNode()  == nid)
					e->setColor(Qt::red);

			set_delay(msec_delay);
			nid->setColor(Qt::red);
			is_visited[list.indexOf(nid)] = true;
		}
	}

	QMessageBox::information(this, " ", "Vertex not found!", QDialogButtonBox::Ok);
	slider->close();
	clearFormat();
}

void GraphWindow::closeEvent(QCloseEvent * event)
{
	if (processing)
	{
		int b = QMessageBox::question(this,"?","Algorithm not over yet. Force stop?",QMessageBox::Yes,QMessageBox::No);
		if(b == QMessageBox::Yes)
			processing = false;
		event->ignore();
		return;
	}
	
	event->accept();
	writeSettings();
}

void GraphWindow::floyd_alg()
{
	size = getMscene()->nodesList().size();

	QTableWidget * tbl = new QTableWidget(size,size);

	QStringList lst;
	foreach (Node * n, getMscene()->nodesList())
		lst << n->get_val();

	tbl->setHorizontalHeaderLabels(lst);
	tbl->setVerticalHeaderLabels(lst);


	floyd_brows = new FloydHelp(tbl);
	floyd_brows->show();
	floyd_brows->hide();
	QThread * thread = new QThread;
	FloydCalc * c = new FloydCalc(getMscene());
	c->moveToThread(thread);

	connect(thread,&QThread::started,c, &FloydCalc::calculation);
	connect(c,&FloydCalc::finished,thread,&QThread::quit);

	connect(c,&FloydCalc::sendInfo,this,&GraphWindow::get_tbl_wid);

	connect(thread,&QThread::finished,this,&GraphWindow::floyd_final,Qt::QueuedConnection);

	thread->start();
}

void showHelp(QTableWidget * tbl,InformationDialog * h)
{
//	h->exec();
}

void GraphWindow::floyd_final()
{
	floyd_brows->show();

	//QThread * sec_thread = new QThread;

	//FloydCalc * c_second = new FloydCalc(tbl,h);


	//c_second->moveToThread(sec_thread);


	//connect(sec_thread,&QThread::started,c_second,&FloydCalc::showHelp);
	//connect(c_second,&FloydCalc::finished,sec_thread,&QThread::quit);
//sec_thread->start();
}

void GraphWindow::get_tbl_wid(QString str)
{
	QStringList list = str.split("|");
	int column = list[0].toInt();
	int raw = list[1].toInt();

	QTableWidgetItem * ptwi = new QTableWidgetItem(list[2].trimmed());
	ptwi->setTextAlignment(Qt::AlignCenter);
	ptwi->setFlags(Qt::ItemIsSelectable |Qt::ItemIsEnabled);

	floyd_brows->setTableItem(ptwi,column,raw);
}

void GraphWindow::writeSettings()
{
	QSettings settings("LETI", "Graph application");
	settings.setValue("geometry",saveGeometry());
}

void GraphWindow::readSettings()
{
	QSettings settings("LETI", "Graph application");
	restoreGeometry(settings.value("geometry").toByteArray());
}

void GraphWindow::hamilton_alg()
{
	if(getMscene()->nodesList().isEmpty())
		return;
	if(getMscene()->isOriented())
	{
		QMessageBox::warning(this, "!", "Graph must be non-oriented (can be changed in settings)", QMessageBox::Ok);

		m_nodes.clear();
		return;
	}

	size= getMscene()->nodesList().size();
	foreach (Node* node, getMscene()->nodesList())
		m_nodes.push_back(node->get_val());

	processing = true;

	QSlider * slider = new QSlider(Qt::Horizontal); //slide for controlling the delay
	slider->setRange(1,300);
	slider->setValue(50);
	slider->setSingleStep(1);
	slider->setPageStep(1);
	slider->setTickInterval(50);
	slider->setMinimumWidth(160);
	slider->setTickPosition(QSlider::TicksBelow);
	slider->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
	slider->setWindowModality(Qt::ApplicationModal);
	slider->setWindowTitle("Задержка алгоритма");
	slider->move(this->pos());
	slider->show();
	connect(slider,&QSlider::valueChanged,this,&GraphWindow::setDelay);


	QString res("");
	generate(0,res);
	slider->close();
	InformationDialog::outDialog(res,"Гамильтонов путь");
}

bool GraphWindow::gamPyt()
{
	for(int i = 0; i < size-1; i++)
	{
		Node * a = getMscene()->findNode(m_nodes[i]);
		Node * b = getMscene()->findNode(m_nodes[i+1]);
		Edge * e = getMscene()->findEdge(a,b);
		a->setColor(Qt::cyan);
		b->setColor(Qt::cyan);

		if(!e)
		{
			clearFormat();
			return false;
		}
		e->setColor(Qt::cyan);
		set_delay(msec_delay);


	}
	return true;
}

void GraphWindow::generate(int k, QString &res)
{
	if(!processing)
		return;
	gamPyt();
	if (k == size)
	{

			   if (gamPyt() == true)
			   {
				   processing =false;
				   for(int i = 0;i < size;i++)
					   res += m_nodes[i] + " ";
				   res += "   -  Hamilton path\n";
			   }
	}
	 else
	{
		for(int j = k;j < size;j++)
			 {

			   qSwap(m_nodes[k],m_nodes[j]);
			   generate(k+1,res);
			   qSwap(m_nodes[k],m_nodes[j]);
		  }
	}
}

void GraphWindow::writePathSettings(QString str)
{
	QFileInfo inf(str);
	QString b = inf.absoluteDir().absolutePath();
	QSettings settings("LETI", "Graph application");
	settings.setValue("path",b);
}


QString GraphWindow::readPathSettings()
{
	QSettings settings("LETI", "Graph application");
	QString str = settings.value("path").toString();
	return str;
}

