#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Headers/secondwindow.h"
#include "Headers/geodesicfunctions.h"
#include "Headers/plottingfunctions.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int lox_index = 0; // Прошлые индексы размерности локсодромы и ортодромы соответственно (для перевода км в мили)
    int orth_index = 0;

private slots:

    void on_calculate_course_button_clicked();

    void on_action_triggered();

    void on_action_2_triggered();

    void on_units_of_loxodrome_measurement_box_currentIndexChanged(int index);

    void on_units_of_orthodrome_measurement_box_currentIndexChanged(int index);

    void GetCoordinates(QVector<QVector<double>>, QColor);

private:
    Ui::MainWindow *ui;
    SecondWindow *sw;

    GeodesicFunctions gf;
    PlottingFunctions *pf;
};
#endif // MAINWINDOW_H
