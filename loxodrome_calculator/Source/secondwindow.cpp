#include "Headers/secondwindow.h"
#include "ui_secondwindow.h"
#include "QMessageBox"

SecondWindow::SecondWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SecondWindow)
{
    ui->setupUi(this);
}

SecondWindow::~SecondWindow()
{
    delete ui;
}

void SecondWindow::on_calculate_longitude_button_clicked()
{
    double start_latitude = ui->start_latitude_line->text().toDouble() * M_PI/180; // Задаем значения и сразу переводим в радианы
    if (ui->start_latitude_hemisphere_box->currentIndex() == 1) {start_latitude = gf.SouthToNorth(start_latitude);} //Теперь все значения переведем в с.ш. и в.д.
    if (start_latitude == M_PI/2 || start_latitude == -M_PI/2) {start_latitude = start_latitude * 0.999;} // Для конечности результатов, не должно быть полюсов, поэтому слегка их уменьшаем (точности это почти не вредит)

    double assigned_latitude = ui->assigned_latitude_line->text().toDouble() * M_PI/180;
    if (ui->assigned_latitude_hemisphere_box->currentIndex() == 1) {assigned_latitude = gf.SouthToNorth(assigned_latitude);}
    if (assigned_latitude == M_PI/2 || assigned_latitude == -M_PI/2) {assigned_latitude = assigned_latitude * 0.999;}

    double start_longitude = ui->start_longitude_line->text().toDouble() * M_PI/180;
    if (ui->start_longitude_hemisphere_box->currentIndex() == 1) {start_longitude = gf.WestToEast(start_longitude);}

    double course = ui->course_line->text().toDouble() * M_PI/180;


    //Теперь определяем долготу точки по начальной точке и запнному курсу

    double unknown_longitude; // Искомая долгота

    if (((assigned_latitude >= start_latitude) && (cos(course) >= 0)) || (((assigned_latitude <= start_latitude) && (cos(course) < 0)))) // Проверка на принадлежность точки к локсодроме (ЭТО НАДО ДОРАБОТАТЬ ТК МОЖЕТ БЫТЬ НАОБОРОТ)
    {
        unknown_longitude = gf.FindLongitude(start_latitude, assigned_latitude, start_longitude, course); // Ищем долготу

        if (unknown_longitude < 0) // Условие для перевода в.д. в з.д.
        {
            ui->unknown_longitude_line->setText(QString::number(-1 * unknown_longitude * 180/M_PI));
            ui->unknown_longitude_hemisphere_box->setCurrentIndex(1);
        }

        else
        {
            ui->unknown_longitude_line->setText(QString::number(unknown_longitude  * 180/M_PI));
            ui->unknown_longitude_hemisphere_box->setCurrentIndex(0);
        }


        // Определим длину пути локсодромы с найденный курсом

        double loxodrome_length = gf.FindLoxodromeLength(start_latitude, assigned_latitude, start_longitude, unknown_longitude, course);

        loxodrome_length = gf.UnitsOfMeasurementChanging(lox_index, ui->units_of_loxodrome_measurement_box->currentIndex(), loxodrome_length);

        ui->loxodrome_length_line->setText(QString::number(loxodrome_length));


        // Определим длину пути ортодромы

        double orthodrome_length = gf.FindOrthodromeLength(start_latitude, assigned_latitude, start_longitude, unknown_longitude);

        orthodrome_length = gf.UnitsOfMeasurementChanging(orth_index, ui->units_of_orthodrome_measurement_box->currentIndex(), orthodrome_length);

        ui->orthodrome_length_line->setText(QString::number(orthodrome_length));


        if (start_latitude == assigned_latitude ) // Условия при совпадении точек (тоже не понятно зачем, но пусть будет)
        {
            ui->orthodrome_length_line->setText("-");

            ui->loxodrome_length_line->setText("-");

            ui->unknown_longitude_line->setText("-");
        }


        // Строим локсодрому
        QVector<QVector<double>> loxodrome_coordinates = gf.FindLoxodromePoints(start_latitude, assigned_latitude, start_longitude, unknown_longitude, course);

        emit SendCoordinates(loxodrome_coordinates, QColor(255, 0, 0));


        // Строим ортодрому
        QVector<QVector<double>> orthodrome_coordinates = gf.FindOrthodromePoints(start_latitude, assigned_latitude, start_longitude, unknown_longitude);

        emit SendCoordinates(orthodrome_coordinates, QColor(0, 255, 0));

    }

    else // Если не лежит на локсодроме
    {
        QMessageBox::warning(this, "", "Искомая точка не лежит на заданной локсодроме");
    }

}


void SecondWindow::on_action_2_triggered()
{

}


void SecondWindow::on_action_triggered() // Смена второго окна на первое
{
    this->hide();

    emit OpenMainWindow();
}


void SecondWindow::on_units_of_loxodrome_measurement_box_currentIndexChanged(int new_index) // Изменение единиц измерения длины локсодромы
{
    ui->loxodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(lox_index, new_index, ui->loxodrome_length_line->text().toDouble())));

    lox_index = new_index;
}


void SecondWindow::on_units_of_orthodrome_measurement_box_currentIndexChanged(int new_index) // Изменение единиц измерения длины ортодромы
{
    ui->orthodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(orth_index, new_index, ui->orthodrome_length_line->text().toDouble())));

    orth_index = new_index;
}

