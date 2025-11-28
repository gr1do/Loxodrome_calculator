#include "Headers/secondwindow.h"
#include "ui_secondwindow.h"
#include "QMessageBox"

SecondWindow::SecondWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SecondWindow)
{
    ui->setupUi(this);

    table_model = new QStandardItemModel;

    table_model->setHorizontalHeaderLabels({"Широта", "Долгота (численная)", "Долгота (аналитическая)", "Разница долгот"});
    ui->loxodromes_coordinate_table->setModel(table_model);

    // Устанавливаем ограничение на ввод значений
    QRegularExpressionValidator *latitude_validator = new QRegularExpressionValidator(QRegularExpression("^(90(\\.0{1,3})?|([0-8]?[0-9])(\\.\\d{1,3})?)$"));
    QRegularExpressionValidator *longitude_validator = new QRegularExpressionValidator(QRegularExpression("^(180(\\.0{1,3})?|([1]?[0-7][0-9])(\\.\\d{1,3})?)$"));
    QRegularExpressionValidator *course_validator = new QRegularExpressionValidator(QRegularExpression("^("
                                                                                                       "(3[0-5][0-9])(\\.\\d{1,3})?|"
                                                                                                       "([1-2][0-9][0-9])(\\.\\d{1,3})?|"
                                                                                                       "([1-9][0-9])(\\.\\d{1,3})?|"
                                                                                                       "([0-9])(\\.\\d{1,3})?"
                                                                                                       ")$"));

    ui->start_longitude_line->setValidator(longitude_validator);
    ui->start_latitude_line->setValidator(latitude_validator);
    ui->assigned_latitude_line->setValidator(latitude_validator);
    ui->delta_longitude_line->setValidator(longitude_validator);
    ui->course_line->setValidator(course_validator);
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
        loxodrome_length = gf.UnitsOfMeasurementChanging(lox_index, 0, loxodrome_length);
        QVector<QVector<double>> loxodrome_coordinates = gf.FindLoxodromePoints(start_latitude, assigned_latitude, start_longitude, unknown_longitude, course, loxodrome_length);
        emit SendCoordinates(loxodrome_coordinates, QColor(255, 0, 0));


        // Строим численную локсодрому
        QVector<QVector<double>> numerical_loxodrome_coordinates = gf.FindNumericalLoxodromePoints(start_latitude, assigned_latitude, start_longitude, unknown_longitude, course, ui->delta_longitude_line->text().toDouble() * M_PI / 180);
        emit SendCoordinates(numerical_loxodrome_coordinates, QColor(255, 255, 0));

        // Выводим значения в таблицу
        QVector<QVector<double>> numerical_loxodrome_spheric_coordinates = gf.GetLatitudeAndLongitudeFromXZY(numerical_loxodrome_coordinates);
        QVector<double> loxodrome_longitudes;

        for (int i = 0; i < numerical_loxodrome_spheric_coordinates[0].length(); i++)
        {
            loxodrome_longitudes.append(gf.FindLongitude(start_latitude, numerical_loxodrome_spheric_coordinates[1][i], start_longitude, course));
        }

        table_model->setRowCount(loxodrome_longitudes.length());

        for(int i = 0; i < numerical_loxodrome_spheric_coordinates[0].length(); i++)
        {
            table_model->setItem(i, 0, new QStandardItem(QString::number(numerical_loxodrome_spheric_coordinates[1][i] * 180 / M_PI)));
            table_model->setItem(i, 1, new QStandardItem(QString::number(numerical_loxodrome_spheric_coordinates[0][i] * 180 / M_PI)));
            table_model->setItem(i, 2, new QStandardItem(QString::number(loxodrome_longitudes[i] * 180 / M_PI)));

            double delta_longitude; // Оприделяем разницу долгот
            if (abs(numerical_loxodrome_spheric_coordinates[0][i] - loxodrome_longitudes[i]) <= M_PI)
            {
                delta_longitude = numerical_loxodrome_spheric_coordinates[0][i] - loxodrome_longitudes[i];
            }
            if (numerical_loxodrome_spheric_coordinates[0][i] - loxodrome_longitudes[i] > M_PI)
            {
                delta_longitude = - (2 * M_PI - (numerical_loxodrome_spheric_coordinates[0][i] - loxodrome_longitudes[i]));
            }
            if (numerical_loxodrome_spheric_coordinates[0][i] - loxodrome_longitudes[i] < -M_PI)
            {
                delta_longitude = 2 * M_PI - abs(numerical_loxodrome_spheric_coordinates[0][i] - loxodrome_longitudes[i]);
            }

            table_model->setItem(i, 3, new QStandardItem(QString::number((delta_longitude) * 180 / M_PI)));
        }


        // Строим ортодрому
        orthodrome_length = gf.UnitsOfMeasurementChanging(orth_index, 0, orthodrome_length);
        QVector<QVector<double>> orthodrome_coordinates = gf.FindOrthodromePoints(start_latitude, assigned_latitude, start_longitude, unknown_longitude, orthodrome_length);
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


void SecondWindow::on_reset_camera_clicked()
{
    ResetCamera();
}

