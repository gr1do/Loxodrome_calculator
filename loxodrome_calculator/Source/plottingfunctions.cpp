#include "Headers/plottingfunctions.h"
#include "cmath"
#include "QDebug"

PlottingFunctions::PlottingFunctions(QObject *parent)
    : QObject(parent)
    , surface(nullptr)
    , quickWidget(nullptr)
{
    SetUp();
}


void PlottingFunctions::SetUp()
{
    quickWidget = new QQuickWidget();

    surface = new Q3DSurfaceWidgetItem();
    surface->setWidget(quickWidget);
    surface->widget()->setMinimumSize(QSize(1024, 1024));

    surface->axisX()->setRange(-7000, 7000);
    surface->axisY()->setRange(-7000, 7000);
    surface->axisZ()->setRange(-7000, 7000);

    surface->axisX()->setTitle("Ось X");
    surface->axisY()->setTitle("Ось Y");
    surface->axisZ()->setTitle("Ось Z");
    surface->axisX()->setTitleVisible(1);
    surface->axisY()->setTitleVisible(1);
    surface->axisZ()->setTitleVisible(1);



    surface->setAspectRatio(1);
}


void PlottingFunctions::PlotSphere()
{
    QSurfaceDataArray data;
    QVector<QSurfaceDataRow> Rows;
    QSurfaceDataRow dataRow;


    for (int teta = 0; teta <= 18; teta++)
    {
        for (int phi = 0; phi <= 36; phi++)
        {
            dataRow << QSurfaceDataItem((float)(6371 * (cos(phi*M_PI/18)*sin(teta*M_PI/18))), (float)(6371 * cos(teta*M_PI/18)), (float)(6371 * (sin(phi*M_PI/18)*sin(teta*M_PI/18))));
        }

        Rows.append(dataRow);
        dataRow.clear();
    }

    for (int i = 0; i < Rows.length(); i++)
    {
        data << Rows[i];
    }

    QSurface3DSeries *series = new QSurface3DSeries();
    series->dataProxy()->resetArray(data);

    //series->setDrawMode(QSurface3DSeries::DrawSurface); // Убираем сетку
    series->setBaseColor(QColor(100, 170, 255)); // Цвет сферы

    surface->addSeries(series);
}


void PlottingFunctions::PlotTrajectory(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y)
{
    for (int i = 0; i < X.length(); i++)
    {
        QSurfaceDataArray data;
        QVector<QSurfaceDataRow> Rows;
        QSurfaceDataRow dataRow;

        data.clear();
        Rows.clear();
        dataRow.clear();

        for (int teta = 0; teta <= 18; teta+=9)
        {
            for (int phi = 0; phi <= 36; phi+=9)
            {
                dataRow << QSurfaceDataItem((float)(6371 * (cos(phi*M_PI/18)*sin(teta*M_PI/18) *0.02 + X[i])), (float)(6371 * (sin(phi*M_PI/18)*sin(teta*M_PI/18) *0.02 + Y[i])), (float)(6371 * (cos(teta*M_PI/18) *0.02 + Z[i])));

                //qDebug() << X[i] << Z[i] << Y[i] << i;

            }

            Rows.append(dataRow);
            dataRow.clear();
        }

        for (int i = 0; i < Rows.length(); i++)
        {
            data << Rows[i];
        }
        QSurface3DSeries *series = new QSurface3DSeries();

        all_series.append(series);

        //series->clearArray();
        series->dataProxy()->resetArray(data);

        series->setDrawMode(QSurface3DSeries::DrawSurface); // Убираем сетку
        series->setBaseColor(QColor(255, 0, 0)); // Цвет точек траектории

        qDebug() << "Создал серию" << i;
        surface->addSeries(series);
        qDebug() << "Добавил серию";
    }
}


void PlottingFunctions::RePlot(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y)
{
    for (auto series : all_series) // Удаляем предыдущие траектории и очищаем массив, где они хранятся
    {
        qDebug() << "Что-то удалил";
        surface->removeSeries(series);
        delete series;
    }

    all_series.clear();
    qDebug() << "Дошел до построения точек";
    PlotTrajectory(X, Z, Y);

    //surface->widget()->show();
}
