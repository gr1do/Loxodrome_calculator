#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "secondwindow.h"
#include "geodesicfunctions.h"

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

    int lox_index = 0;
    int orth_index = 0;

private slots:

    void on_calculate_course_button_clicked();

    void on_action_triggered();

    void on_action_2_triggered();

    void on_units_of_loxodrome_measurement_box_currentIndexChanged(int index);

    void on_units_of_orthodrome_measurement_box_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    SecondWindow *sw;

    GeodesicFunctions gf;
};
#endif // MAINWINDOW_H
