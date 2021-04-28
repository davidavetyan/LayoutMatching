#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LayoutMatching.h"

#include <QPointer>

class GraphWindow;

class LayoutMatching : public QMainWindow
{
	Q_OBJECT

public:
	LayoutMatching(QWidget *parent = Q_NULLPTR);

	void ReadRectsFromFile(QString const& sFilename);
	void CalculateIntersections();

	void CalculateMatching();

	void paintEvent(QPaintEvent* event) override;
	void DrawRects(QPainter* painter);

public slots:
	void OnCalculateMatching();

private:
	Ui::LayoutMatchingClass ui;
	QVector<QRectF> m_vecRects;
	QVector<QRectF> m_vecIntersections;
	QPointer<GraphWindow> m_pGraphView;
};
