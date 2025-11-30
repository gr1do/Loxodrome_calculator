#include "Headers/graphswindow.h"
#include "ui_graphswindow.h"
#include "QDebug"

GraphsWindow::GraphsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphsWindow)
{
    ui->setupUi(this);

    CustomizeAbsoluteGraph(ui->absolute_graph_widget);

    CustomizeDifferenceGraph(ui->difference_graph_widget);
}

GraphsWindow::~GraphsWindow()
{
    delete ui;
}

void GraphsWindow::PlotAbsoluteGraph(QVector<double> longitudes, QVector<double> latitudes, int index)
{
    ui->absolute_graph_widget->graph(index)->data()->clear(); // Очищаем старый график
    ui->absolute_graph_widget->replot();

    QVector<double> degrees_longitudes;
    QVector<double> degrees_latitudes;
    for (int i = 0; i < longitudes.length(); i++) // Переводим из радиан в градусы и добавляем на график
    {
        degrees_longitudes.append(longitudes[i] * 180 / M_PI);
        degrees_latitudes.append(latitudes[i] * 180 / M_PI);
    }

    ui->absolute_graph_widget->graph(index)->addData(degrees_longitudes, degrees_latitudes);
    ui->absolute_graph_widget->replot();
}

void GraphsWindow::PlotLatitudeDifferenceGraph(QVector<double> longitudes, QVector<double> latitudes, QVector<double> numerical_latitudes)
{
    ui->difference_graph_widget->graph(0)->data()->clear(); // Очищаем старый график
    ui->difference_graph_widget->replot();

    QVector<double> degrees_longitudes;
    QVector<double> degrees_latitudes_difference;

    for (int i = 0; i < longitudes.length(); i++)
    {
        degrees_longitudes.append(longitudes[i] * 180 / M_PI);
        degrees_latitudes_difference.append((numerical_latitudes[i] - latitudes[i]) * 180 / M_PI);
    }

    ui->difference_graph_widget->xAxis->setRange(-180, 180); // Устанавливаем границы
    ui->difference_graph_widget->yAxis->setRange(-90, 90);

    ui->difference_graph_widget->xAxis->setLabel("Долгота");
    ui->difference_graph_widget->yAxis->setLabel("Разница широт");

    ui->difference_graph_widget->graph(0)->addData(degrees_longitudes, degrees_latitudes_difference);
    ui->difference_graph_widget->replot();
}

void GraphsWindow::PlotLongitudeDifferenceGraph(QVector<double> latitudes, QVector<double> delta_longitudes)
{
    ui->difference_graph_widget->graph(0)->data()->clear(); // Очищаем старый график
    ui->difference_graph_widget->replot();

    QVector<double> degrees_latitudes;
    QVector<double> degrees_longitudes_difference;

    for (int i = 0; i < delta_longitudes.length(); i++)
    {
        degrees_latitudes.append(latitudes[i] * 180 / M_PI);
        degrees_longitudes_difference.append(delta_longitudes[i] * 180 / M_PI);
    }

    ui->difference_graph_widget->xAxis->setRange(-90, 90); // Устанавливаем границы
    ui->difference_graph_widget->yAxis->setRange(-180, 180);

    ui->difference_graph_widget->xAxis->setLabel("Широта");
    ui->difference_graph_widget->yAxis->setLabel("Разница долгот");

    ui->difference_graph_widget->graph(0)->addData(degrees_latitudes, degrees_longitudes_difference);
    ui->difference_graph_widget->replot();
}

void GraphsWindow::CustomizeAbsoluteGraph(QCustomPlot *plot)
{
    plot->addGraph(); // Добавляем графики
    plot->addGraph();
    plot->addGraph();

    plot->xAxis->setRange(-180, 180); // Устанавливаем границы
    plot->yAxis->setRange(-90, 90);

    plot->xAxis->setLabel("Долгота");
    plot->yAxis->setLabel("Широта");

    plot->setBackground(QBrush(Qt::black)); // Черный фон

    plot->xAxis->setTickPen(QPen(Qt::white, 1));
    plot->yAxis->setTickPen(QPen(Qt::white, 1));
    plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->yAxis->setSubTickPen(QPen(Qt::white, 1));

    // Белый текст осей
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->xAxis->setLabelColor(Qt::white);
    plot->yAxis->setLabelColor(Qt::white);


    plot->graph(0)->setLineStyle(QCPGraph::lsNone); // Настраиваем график аналитической локсодромы
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QPen(QColor(255, 0, 0)), QBrush(QColor(255, 0, 0)), 2.5));

    plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QPen(QColor(255, 255, 0)), QBrush(QColor(255, 255, 0)), 2)); // Настраиваем график численной локсодромы
    plot->graph(1)->setLineStyle(QCPGraph::lsNone);

    plot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QPen(QColor(0, 255, 0)), QBrush(QColor(0, 255, 0)), 2.5)); // Настраиваем график ортодромы
    plot->graph(2)->setLineStyle(QCPGraph::lsNone);
}

void GraphsWindow::CustomizeDifferenceGraph(QCustomPlot *plot)
{
    plot->addGraph(); // Добавляем графики

    plot->setBackground(QBrush(Qt::black)); // Черный фон

    plot->xAxis->setTickPen(QPen(Qt::white, 1));
    plot->yAxis->setTickPen(QPen(Qt::white, 1));
    plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->yAxis->setSubTickPen(QPen(Qt::white, 1));

    // Белый текст осей
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->xAxis->setLabelColor(Qt::white);
    plot->yAxis->setLabelColor(Qt::white);

    plot->graph(0)->setLineStyle(QCPGraph::lsNone); // Настраиваем график разницы
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QPen(QColor(0, 255, 255)), QBrush(QColor(0, 255, 255)), 2.5));

}
