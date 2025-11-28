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


    BackCamera(); // Начальное положение камеры

    surface->setMinCameraYRotation(-360); // Убираем ограниечение поворота
    surface->setMaxCameraYRotation(360);

    surface->setSelectionEnabled(false); // Выключаем возможность выделения, т.к. это не информативно из-за ДСК
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


    // Делаем экватор и меридианы

    // Экватор
    QVector<QVector<double>> ecuator_coordinates = gf.FindEcuatorPoints();
    PlotTrajectory(ecuator_coordinates[0], ecuator_coordinates[1], ecuator_coordinates[2], QColor(0, 0, 0));

    // Меридианы
    QVector<QVector<double>> greenwich_coordinates = gf.FindGreenwichPoints();
    PlotTrajectory(greenwich_coordinates[0], greenwich_coordinates[1], greenwich_coordinates[2], QColor(255, 0, 255)); // Гринвич
    PlotTrajectory(greenwich_coordinates[3], greenwich_coordinates[4], greenwich_coordinates[5], QColor(0, 0, 0)); // Три других меридиана
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


        for (int phi = 0; phi <= 36; phi+=6) // Просчитываем точки каждой маленькой фигуры, которой обозначаем траекторию
        {
            for (double teta = 0; teta <= 18; teta+=3)
            {
                dataRow << QSurfaceDataItem((float)(6330 * (cos(phi*M_PI/18)*sin(teta*M_PI/18) *0.02 + X[i])), (float)(6330 * (sin(phi*M_PI/18)*sin(teta*M_PI/18) *0.02 + Y[i])), (float)(6330 * (cos(teta*M_PI/18) *0.02 + Z[i])));
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

        if (color == QColor(255, 255, 0)) {all_numerical_loxodrome_series.append(series);} // Условие численной локсодромы (желтый)

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

    if (color == QColor(255, 255, 0)) // Если локсодрома (красный)
    {
        for (auto series : all_numerical_loxodrome_series) // Удаляем предыдущую траекторию численной локсодромы и очищаем массив, где она хранятся
        {
            surface->removeSeries(series);
            delete series;
        }

        all_numerical_loxodrome_series.clear();
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

void PlottingFunctions::BackCamera() // Возвращение камеры в начальное положение
{
    surface->setCameraXRotation(-90);
    surface->setCameraYRotation(0);

    surface->activeTheme()->update();
}
