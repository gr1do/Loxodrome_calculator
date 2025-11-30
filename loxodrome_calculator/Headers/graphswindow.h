#ifndef GRAPHSWINDOW_H
#define GRAPHSWINDOW_H

#include <QMainWindow>
#include "Headers/qcustomplot.h"

namespace Ui {
class GraphsWindow;
}

class GraphsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphsWindow(QWidget *parent = nullptr);
    ~GraphsWindow();

    void PlotAbsoluteGraph(QVector<double>, QVector<double>, int);

    void PlotLatitudeDifferenceGraph(QVector<double>, QVector<double>, QVector<double>);

    void PlotLongitudeDifferenceGraph(QVector<double>, QVector<double>);

    void CustomizeAbsoluteGraph(QCustomPlot*);

    void CustomizeDifferenceGraph(QCustomPlot*);

private:
    Ui::GraphsWindow *ui;
};

#endif // GRAPHSWINDOW_H
