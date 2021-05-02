#include "LayoutMatching.h"

#include "algorithms.h"
#include "graphwindow.h"

#include <QFile>
#include <QPainter>
#include <QTextStream>
#include <QMessageBox>

#include <vector>

LayoutMatching::LayoutMatching(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_pGraphView = new GraphWindow;

	connect(ui.calculateButton, &QPushButton::pressed, this, &LayoutMatching::OnCalculateMatching);
	connect(ui.actRectIndexes, &QAction::toggled, this, &LayoutMatching::OnShowIndexesToggled);
	connect(ui.actFileOpen, &QAction::triggered, this, &LayoutMatching::OnOpenFile);
}

void LayoutMatching::paintEvent(QPaintEvent* event)
{
	//QPainter painter(this);
	//DrawRegions(&painter);

	//if (m_pCurrentPath)
	//	painter.drawPath(*m_pCurrentPath);

	//if (m_bFirstPress && !m_bSecondPress)
	//{
	//	painter.setPen(QPen(Qt::white, 1, Qt::DotLine, Qt::FlatCap));
	//	painter.drawLine(m_point1, m_point2);
	//}
	//if (m_bFirstPress && m_bSecondPress)
	//{
	//	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap));
	//	painter.drawLine(m_point1, m_point2);
	//	m_bSecondPress = false;
	//}
}

void LayoutMatching::GetMatrixFromRegions(QVector<QPainterPath> const& vecRegions, std::vector<std::vector<int>>& G)
{
	G.resize(vecRegions.size());
	for (auto& row : G)
		row.resize(vecRegions.size());

	for (int i = 0; i < vecRegions.size(); i++)
	{
		G[i][i] = 0;
		for (int j = i + 1; j < vecRegions.size(); j++)
		{
			int intersectArea = Region::getPainterPathArea(vecRegions[i].intersected(vecRegions[j]));
			G[i][j] = intersectArea;
			G[j][i] = intersectArea;
		}
	}
}

void LayoutMatching::CalculateMatching()
{
	QVector<QPainterPath> const& vecRegions = ui.canvasWidget->GetRegions();
	std::vector<std::vector<int>> G;

	GetMatrixFromRegions(vecRegions, G);

	std::pair<std::vector<int>, std::vector<int>> bipartitionIndexes;
	if (!Graph::isBipartite(G, bipartitionIndexes))
	{
		QMessageBox::information(this, "Error", "Graph created from the layout isn't bipartite");
		return;
	}

	std::vector<std::vector<int>> adjMatrix;
	Graph::createAdjFromBipartition(G, bipartitionIndexes, adjMatrix);

	std::vector<std::vector<int>> hungarianMask;
	Graph::Hungarian::solveHungarian(adjMatrix, hungarianMask);

	std::vector<std::pair<int, int>> edges;
	Graph::Hungarian::getEdgesFromHungarian(adjMatrix, bipartitionIndexes, hungarianMask, edges);

	if (!m_pGraphView)
		m_pGraphView = new GraphWindow;
	m_pGraphView->createGraphFromMatrix(G, &edges);

	m_pGraphView->resize(600, 600);
	m_pGraphView->show();
}

void LayoutMatching::OnCalculateMatching()
{
	CalculateMatching();
}

void LayoutMatching::OnOpenFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Regions File"), "", tr("All Files (*)"));
	
	QFile f(fileName);
	auto regions = Region::readFromFile(f);

	ui.canvasWidget->SetRegions(regions);
}

void LayoutMatching::OnShowIndexesToggled(bool bChecked)
{
	ui.canvasWidget->SetShowIndexes(bChecked);
}
