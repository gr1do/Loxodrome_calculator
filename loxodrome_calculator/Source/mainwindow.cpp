#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cmath"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sw = new SecondWindow;

    connect(sw, &SecondWindow::OpenMainWindow, this, &MainWindow::show);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_calculate_course_button_clicked()
{
    double start_latitude = ui->start_latitude_line->text().toDouble() * M_PI/180; // Задаем значения и сразу переводим в радианы
    if (ui->start_latitude_hemisphere_box->currentIndex() == 1) {start_latitude = gf.SouthToNorth(start_latitude);} //Теперь все значения переведем в с.ш. и в.д.

    double start_longitude = ui->start_longitude_line->text().toDouble() * M_PI/180;
    if (ui->start_longitude_hemisphere_box->currentIndex() == 1) {start_longitude = gf.WestToEast(start_longitude);}

    double end_latitude = ui->end_latitude_line->text().toDouble() * M_PI/180;
    if (ui->end_latitude_hemisphere_box->currentIndex() == 1) {end_latitude = gf.SouthToNorth(end_latitude);}

    double end_longitude = ui->end_longitude_line->text().toDouble() * M_PI/180;
    if (ui->end_longitude_hemisphere_box->currentIndex() == 1) {end_longitude = gf.WestToEast(end_longitude);}


    // Определяем курс по двум точкам

    double course = gf.FindCourse(start_latitude, end_latitude, start_longitude, end_longitude);

    ui->course_line->setText(QString::number(course*180/M_PI));


    // Определим длину локсодромы с найденный курсом

    double loxodrome_length = gf.FindLoxodromeLength(start_latitude, end_latitude, start_longitude, end_longitude, course);

    loxodrome_length = gf.UnitsOfMeasurementChanging(lox_index, ui->units_of_loxodrome_measurement_box->currentIndex(), loxodrome_length);

    ui->loxodrome_length_line->setText(QString::number(loxodrome_length));

    if ((start_latitude == M_PI/2 && end_latitude == M_PI/2) || (start_latitude == -M_PI/2 && end_latitude == -M_PI/2)) // Условия при совпадении полюсов
    {
        ui->course_line->setText("-");
        ui->loxodrome_length_line->setText("-");
    }


    // Определим длину пути ортодромы

    double orthodrome_length = gf.FindOrthodromeLength(start_latitude, end_latitude, start_longitude, end_longitude);

    orthodrome_length = gf.UnitsOfMeasurementChanging(orth_index, ui->units_of_orthodrome_measurement_box->currentIndex(), orthodrome_length);

    ui->orthodrome_length_line->setText(QString::number(orthodrome_length));

    if ((start_latitude == M_PI/2 && end_latitude == M_PI/2) || (start_latitude == -M_PI/2 && end_latitude == -M_PI/2)) // Условия при совпадении полюсов (ПОЧЕМУ ТОЛЬКО ПОЛЮСОВ А НЕ ТОЧЕК???)
    {
        ui->orthodrome_length_line->setText("-");
    }
}


void MainWindow::on_action_2_triggered()
{

}


void MainWindow::on_action_triggered()
{
    this->hide();

    sw->show();
}


void MainWindow::on_units_of_loxodrome_measurement_box_currentIndexChanged(int new_index)
{
    ui->loxodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(lox_index, new_index, ui->loxodrome_length_line->text().toDouble())));

    lox_index = new_index;
}


void MainWindow::on_units_of_orthodrome_measurement_box_currentIndexChanged(int new_index)
{
    ui->orthodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(orth_index, new_index, ui->orthodrome_length_line->text().toDouble())));

    orth_index = new_index;
}

