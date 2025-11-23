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


void PlottingFunctions::SetUp() // Функция для создания и настроки виджета с графиком
{
    quickWidget = new QQuickWidget(); // Привязываем график к виджету

    surface = new Q3DSurfaceWidgetItem();
    surface->setWidget(quickWidget);

    surface->widget()->setMinimumSize(QSize(1024, 1024));


    surface->axisX()->setRange(-7000, 7000); // Усланавливаем диапазон у осей
    surface->axisY()->setRange(-7000, 7000);
    surface->axisZ()->setRange(-7000, 7000);

    surface->setAspectRatio(1); // Делаем оси пропорциональными


    surface->axisX()->setLabelsVisible(0); // Убираем надписи
    surface->axisY()->setLabelsVisible(0);
    surface->axisZ()->setLabelsVisible(0);

    surface->activeTheme()->setGridVisible(0); // Убираем сетку
    surface->activeTheme()->setBackgroundVisible(0); // Убираем какой-то фон


    surface->setCameraXRotation(-90); // Начальное положение камеры

    surface->setMinCameraYRotation(-360); // Убираем ограниечение поворота
    surface->setMaxCameraYRotation(360);
}


void PlottingFunctions::PlotSphere()
{
    QSurfaceDataArray data;
    QVector<QSurfaceDataRow> Rows;
    QSurfaceDataRow dataRow;


    for (int teta = 0; teta <= 18; teta++) // Считаем точки сферы
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

    series->setBaseColor(QColor(100, 170, 255)); // Цвет сферы

    surface->addSeries(series);


    // Делаем экватор и гринвичский меридиан
    // Экватор
    QVector<QVector<double>> ecuator_coordinates = gf.FindEcuatorPoints();
    PlotTrajectory(ecuator_coordinates[0], ecuator_coordinates[1], ecuator_coordinates[2], QColor(0, 0, 0));

    // Гринвич
    QVector<QVector<double>> greenwich_coordinates = gf.FindGreenwichPoints();
    PlotTrajectory(greenwich_coordinates[0], greenwich_coordinates[1], greenwich_coordinates[2], QColor(0, 0, 0));


    // Добавим точку (0; 0)

}


void PlottingFunctions::PlotTrajectory(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y, QColor color) // Функция для построения траектории по массиву заданных точек
{
    for (int i = 0; i < X.length(); i++)
    {
        QSurfaceDataArray data;
        QVector<QSurfaceDataRow> Rows;
        QSurfaceDataRow dataRow;

        data.clear();
        Rows.clear();
        dataRow.clear();


        for (int teta = 0; teta <= 18; teta+=9) // Просчитываем точки каждой маленькой фигуры, которой обозначаем траекторию
        {
            for (int phi = 0; phi <= 36; phi+=9)
            {
                dataRow << QSurfaceDataItem((float)(6400 * (cos(phi*M_PI/18)*sin(teta*M_PI/18) *0.02 + X[i])), (float)(6400 * (sin(phi*M_PI/18)*sin(teta*M_PI/18) *0.02 + Y[i])), (float)(6400 * (cos(teta*M_PI/18) *0.02 + Z[i])));
            }

            Rows.append(dataRow);
            dataRow.clear();
        }

        for (int i = 0; i < Rows.length(); i++)
        {
            data << Rows[i];
        }


        QSurface3DSeries *series = new QSurface3DSeries();

        if (color == QColor(255, 0, 0)) {all_loxodrome_series.append(series);} // Условие локсодромы (красный)

        if (color == QColor(0, 255, 0)) {all_orthodrome_series.append(series);} // Условие ортодромы (зеленый)

        series->dataProxy()->resetArray(data);

        series->setDrawMode(QSurface3DSeries::DrawSurface); // Убираем сетку
        series->setBaseColor(color); // Цвет точек траектории

        surface->addSeries(series);
    }
}


void PlottingFunctions::RePlot(const QVector<double> &X, const QVector<double> &Z, const QVector<double> &Y, QColor color) // Функция для перестроения конкретного графика
{
    if (color == QColor(255, 0, 0)) // Если локсодрома (красный)
    {
    for (auto series : all_loxodrome_series) // Удаляем предыдущую траекторию локсодромы и очищаем массив, где она хранятся
        {
            surface->removeSeries(series);
            delete series;
        }

        all_loxodrome_series.clear();
    }


    if (color == QColor(0, 255, 0)) // Если ортодрома (зеленый)
    {
        for (auto series : all_orthodrome_series) // Удаляем предыдущую траекторию ортодромы и очищаем массив, где она хранятся
        {
            surface->removeSeries(series);
            delete series;
        }

        all_orthodrome_series.clear();
    }


    PlotTrajectory(X, Z, Y, color);
}
