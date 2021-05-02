#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LayoutMatching.h"

#include <QPointer>
#include <QPainterPath>

#include "region.h"

class GraphWindow;

class LayoutMatching : public QMainWindow
{
	Q_OBJECT

public:
	LayoutMatching(QWidget *parent = Q_NULLPTR);

	void CalculateMatching();

	void paintEvent(QPaintEvent* event) override;

	static void GetMatrixFromRegions(QVector<QPainterPath> const& vecRegions, std::vector<std::vector<int>>& G);

public slots:
	void OnCalculateMatching();
	void OnShowIndexesToggled(bool bChecked);
	void OnOpenFile();

private:
	Ui::LayoutMatchingClass ui;
	QPointer<GraphWindow> m_pGraphView;
};
