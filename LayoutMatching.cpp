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

	ReadRectsFromFile("rects.txt");
	CalculateIntersections();

	connect(ui.calculateButton, &QPushButton::pressed, this, &LayoutMatching::OnCalculateMatching);
}

void LayoutMatching::ReadRectsFromFile(QString const& sFilename)
{
	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly))
		return;

	m_vecRects.clear();

	QTextStream in(&file);
	while (!in.atEnd())
	{
		QStringList aRectCoords = in.readLine().split(",");
		if (aRectCoords.size() != 4)
			continue;

		int nLeft = aRectCoords[0].toInt();
		int nTop = aRectCoords[1].toInt();
		int nWidth = aRectCoords[2].toInt();
		int nHeight = aRectCoords[3].toInt();

		m_vecRects.push_back(QRectF(nLeft, nTop, nWidth, nHeight));
	}
}

void LayoutMatching::CalculateIntersections()
{
	m_vecIntersections.clear();
	for (int i = 0; i < m_vecRects.size(); ++i)
	{
		for (int j = i + 1; j < m_vecRects.size(); ++j)
		{
			QRectF rcRes = m_vecRects[i].intersected(m_vecRects[j]);
			if (rcRes.isEmpty())
				continue;

			m_vecIntersections.push_back(rcRes);
		}
	}
}

void LayoutMatching::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	DrawRects(&painter);
}

void LayoutMatching::DrawRects(QPainter* painter)
{
	painter->save();

	painter->setPen(QPen(QBrush(Qt::blue), 3));
	for (auto const& oRect : m_vecRects)
	{
		painter->drawRect(oRect);
		painter->save();

		painter->setPen(QPen(QBrush(Qt::blue), 1.5));

		painter->restore();
		painter->fillRect(oRect, QBrush(Qt::blue, Qt::BDiagPattern));
	}

	painter->setPen(QPen(QBrush(Qt::red), 3));
	for (auto const& oRect : m_vecIntersections)
	{
		painter->drawRect(oRect);
		painter->fillRect(oRect, QBrush(Qt::red, Qt::FDiagPattern));
	}

	painter->restore();
}

void LayoutMatching::CalculateMatching()
{
	constexpr int V = 6;
	std::vector<std::vector<int>> G = 
	{
		{0, 2, 0, 5, 0, 0},
		{2, 0, 20, 0, 0, 0},
		{0, 20, 0, 40, 0, 19},
		{5, 0, 40, 0, 30, 0},
		{0, 0, 0, 30, 0, 0},
		{0, 0, 19, 0, 0, 0},
	};

	std::pair<std::vector<int>, std::vector<int>> bipartitionIndexes;
	if (!Graph::isBipartite(G, bipartitionIndexes))
		QMessageBox::information(this, "Error", "Graph created from the layout isn't bipartite");

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
