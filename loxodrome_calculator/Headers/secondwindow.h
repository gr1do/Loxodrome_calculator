#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QMainWindow>
#include "Headers/geodesicfunctions.h"

namespace Ui {
class SecondWindow;
}

class SecondWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();

    int lox_index = 0;
    int orth_index = 0;

private slots:
    void on_calculate_longitude_button_clicked();

    void on_action_2_triggered();

    void on_action_triggered();

    void on_units_of_loxodrome_measurement_box_currentIndexChanged(int index);

    void on_units_of_orthodrome_measurement_box_currentIndexChanged(int index);

signals:
    void OpenMainWindow();

    void SendCoordinates(QVector<QVector<double>>, QColor);

private:
    Ui::SecondWindow *ui;

    GeodesicFunctions gf;
};

#endif // SECONDWINDOW_H
