#ifndef PLOTTINGFUNCTIONS_H
#define PLOTTINGFUNCTIONS_H

#include <QtGraphsWidgets/q3dsurfacewidgetitem.h>
#include "QVector"
#include "QObject"
#include "Headers/geodesicfunctions.h"
#include "QCustom3DItem"


class PlottingFunctions : public QObject
{
    Q_OBJECT

public:
    explicit PlottingFunctions(QObject *parent = nullptr);

    void SetUp();

    void PlotSphere();

    void PlotTrajectory(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y, QColor);

    void RePlot(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y, QColor);

    void BackCamera();

    Q3DSurfaceWidgetItem *surface; // Виджет графика

    QQuickWidget *quickWidget;

    QVector<QSurface3DSeries*> all_loxodrome_series; // Массив с точками локсодромы

    QVector<QSurface3DSeries*> all_numerical_loxodrome_series; // Массив с точками численной локсодромы

    QVector<QSurface3DSeries*> all_orthodrome_series; // Массив с точками ортодромы

private:
    GeodesicFunctions gf;
};

#endif // PLOTTINGFUNCTIONS_H
