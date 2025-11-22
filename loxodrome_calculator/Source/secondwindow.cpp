#include "Headers/secondwindow.h"
#include "ui_secondwindow.h"

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

    double start_longitude = ui->start_longitude_line->text().toDouble() * M_PI/180;
    if (ui->start_longitude_hemisphere_box->currentIndex() == 1) {start_longitude = gf.WestToEast(start_longitude);}

    double course = ui->course_line->text().toDouble() * M_PI/180;


    //Теперь определяем долготу точки по начальной точке и запнному курсу

    if (ui->assigned_latitude_line->text() != "") // Проверка на наличие значения в лайн эдите (ЭТО НАДО ПЕРЕДЕЛАТЬ В НОРМАЛЬНОМ ВИДЕ!!!)
    {
        double assigned_latitude = ui->assigned_latitude_line->text().toDouble() * M_PI/180;
        if (ui->assigned_latitude_hemisphere_box->currentIndex() == 1) {assigned_latitude = gf.SouthToNorth(assigned_latitude);}

        double unknown_longitude;

        if (((assigned_latitude >= start_latitude) && (cos(course) >= 0)) || (((assigned_latitude <= start_latitude) && (cos(course) < 0)))) // Проверка на принадлежность точки к локсодроме (ЭТО НАДО ДОРАБОТАТЬ ТК МОЖЕТ БЫТЬ НАОБОРОТ)
        {
            unknown_longitude = gf.FindLongitude(start_latitude, assigned_latitude, start_longitude, course);

            if (unknown_longitude < 0) // Переводим в.д. в з.д. и выводим найденную долготу в лайнэдит
            {
                ui->unknown_longitude_line->setText(QString::number(-1 * unknown_longitude * 180/M_PI));
                ui->unknown_longitude_hemisphere_box->setCurrentIndex(1);
            }

            else
            {
                ui->unknown_longitude_line->setText(QString::number(unknown_longitude  * 180/M_PI));
                ui->unknown_longitude_hemisphere_box->setCurrentIndex(0);
            }


            // Определим длину пути с найденный курсом

            double loxodrome_length = gf.FindLoxodromeLength(start_latitude, assigned_latitude, start_longitude, unknown_longitude, course);

            loxodrome_length = gf.UnitsOfMeasurementChanging(lox_index, ui->units_of_loxodrome_measurement_box->currentIndex(), loxodrome_length);

            ui->loxodrome_length_line->setText(QString::number(loxodrome_length));


            // Определим длину пути ортодромы

            double orthodrome_length = gf.FindOrthodromeLength(start_latitude, assigned_latitude, start_longitude, unknown_longitude);

            orthodrome_length = gf.UnitsOfMeasurementChanging(orth_index, ui->units_of_orthodrome_measurement_box->currentIndex(), orthodrome_length);

            ui->orthodrome_length_line->setText(QString::number(orthodrome_length));

            if ((start_latitude == M_PI/2 && assigned_latitude == M_PI/2) || (start_latitude == -M_PI/2 && assigned_latitude == -M_PI/2)) // Условия при совпадении полюсов (ПОЧЕМУ ТОЛЬКО ПОЛЮСОВ А НЕ ТОЧЕК???)
            {
                ui->orthodrome_length_line->setText("-");

                ui->loxodrome_length_line->setText("-");

                ui->unknown_longitude_line->setText("-");
            }
        }

        else
        {
            // QMessageBox::warning(this, "", "Искомая точка не лежит на заданной локсодроме"); // Вывод ошибки (лучше потом переделпть в нормальном виде)
        }
    }
}


void SecondWindow::on_action_2_triggered()
{

}


void SecondWindow::on_action_triggered()
{
    this->hide();

    emit OpenMainWindow();
}


void SecondWindow::on_units_of_loxodrome_measurement_box_currentIndexChanged(int new_index)
{
    ui->loxodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(lox_index, new_index, ui->loxodrome_length_line->text().toDouble())));

    lox_index = new_index;
}


void SecondWindow::on_units_of_orthodrome_measurement_box_currentIndexChanged(int new_index)
{
    ui->orthodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(orth_index, new_index, ui->orthodrome_length_line->text().toDouble())));

    orth_index = new_index;
}

