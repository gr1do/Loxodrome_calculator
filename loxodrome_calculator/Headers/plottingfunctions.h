#ifndef PLOTTINGFUNCTIONS_H
#define PLOTTINGFUNCTIONS_H

#include <QtGraphsWidgets/q3dsurfacewidgetitem.h>
#include "QVector"
// #include <QtGraphs>
#include "QObject"


class PlottingFunctions : public QObject
{
    Q_OBJECT

public:
    explicit PlottingFunctions(QObject *parent = nullptr);

    void SetUp();

    void PlotSphere();

    void PlotTrajectory(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y);

    void RePlot(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y);

    Q3DSurfaceWidgetItem *surface;

    QQuickWidget *quickWidget;

    QVector<QSurface3DSeries*> all_series;
};

#endif // PLOTTINGFUNCTIONS_H
