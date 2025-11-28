#include "Headers/mainwindow.h"
#include "cmath"
#include "QDebug"
#include <QtGraphsWidgets/q3dsurfacewidgetitem.h>
//#include "QDoubleValidator"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sw = new SecondWindow;


    pf = new PlottingFunctions(this);

    pf->PlotSphere();
    pf->surface->widget()->show();


    table_model = new QStandardItemModel;

    table_model->setHorizontalHeaderLabels({"Долгота", "Широта (численная)", "Широта (аналитическая)", "Разница широт"});
    ui->loxodromes_coordinate_table->setModel(table_model);


    connect(sw, &SecondWindow::OpenMainWindow, this, &MainWindow::show);
    connect(sw, &SecondWindow::SendCoordinates, this, &MainWindow::GetCoordinates);
    connect(sw, &SecondWindow::ResetCamera, this, &MainWindow::on_reset_camera_clicked);

    // Устанавливаем ограничение на ввод значений
    QRegularExpressionValidator *latitude_validator = new QRegularExpressionValidator(QRegularExpression("^(90(\\.0{1,3})?|([0-8]?[0-9])(\\.\\d{1,3})?)$"));
    QRegularExpressionValidator *longitude_validator = new QRegularExpressionValidator(QRegularExpression("^(180(\\.0{1,3})?|([1]?[0-7][0-9])(\\.\\d{1,3})?)$"));

    ui->start_longitude_line->setValidator(longitude_validator);
    ui->start_latitude_line->setValidator(latitude_validator);
    ui->end_longitude_line->setValidator(longitude_validator);
    ui->end_latitude_line->setValidator(latitude_validator);
    ui->delta_longitude_line->setValidator(longitude_validator);

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_calculate_course_button_clicked()
{
    //lox_index = 0
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

    if ((start_latitude == M_PI/2 && end_latitude == M_PI/2) || (start_latitude == -M_PI/2 && end_latitude == -M_PI/2)) // Условия при совпадении полюсов (хотя сомнительно, типа зачем? Но пусть будет)
    {
        ui->orthodrome_length_line->setText("-");
    }


    // Строим локсодрому двумя способами
    loxodrome_length = gf.UnitsOfMeasurementChanging(lox_index, 0, loxodrome_length);
    QVector<QVector<double>> loxodrome_coordinates = gf.FindLoxodromePoints(start_latitude, end_latitude, start_longitude, end_longitude, course, loxodrome_length);
    pf->RePlot(loxodrome_coordinates[0], loxodrome_coordinates[1], loxodrome_coordinates[2], QColor(255, 0, 0));

    QVector<QVector<double>> numerical_loxodrome_coordinates = gf.FindNumericalLoxodromePoints(start_latitude, end_latitude, start_longitude, end_longitude, course, ui->delta_longitude_line->text().toDouble() * M_PI/180);
    pf->RePlot(numerical_loxodrome_coordinates[0], numerical_loxodrome_coordinates[1], numerical_loxodrome_coordinates[2], QColor(255, 255, 0));

    // Выводим значения в таблицу
    QVector<QVector<double>> numerical_loxodrome_spheric_coordinates = gf.GetLatitudeAndLongitudeFromXZY(numerical_loxodrome_coordinates); // Задаем массивы сферических коордтнат
    QVector<double> loxodrome_latitudes;

    for (int i = 0; i < numerical_loxodrome_spheric_coordinates[0].length(); i++)
    {
        loxodrome_latitudes.append(gf.FindLatitude(start_latitude, start_longitude, numerical_loxodrome_spheric_coordinates[0][i], course));
    }

    table_model->setRowCount(loxodrome_latitudes.length()); // Изменяем размер таблицы и вводим значения

    for(int i = 0; i < numerical_loxodrome_spheric_coordinates[0].length(); i++)
    {
        table_model->setItem(i, 0, new QStandardItem(QString::number(numerical_loxodrome_spheric_coordinates[0][i] * 180 / M_PI)));
        table_model->setItem(i, 1, new QStandardItem(QString::number(numerical_loxodrome_spheric_coordinates[1][i] * 180 / M_PI)));
        table_model->setItem(i, 2, new QStandardItem(QString::number(loxodrome_latitudes[i] * 180 / M_PI)));
        table_model->setItem(i, 3, new QStandardItem(QString::number((numerical_loxodrome_spheric_coordinates[1][i] - loxodrome_latitudes[i]) * 180 / M_PI)));
    }

    //Строим ортодрому
    orthodrome_length = gf.UnitsOfMeasurementChanging(orth_index, 0, orthodrome_length);
    QVector<QVector<double>> orthodrome_coordinates = gf.FindOrthodromePoints(start_latitude, end_latitude, start_longitude, end_longitude, orthodrome_length);
    pf->RePlot(orthodrome_coordinates[0], orthodrome_coordinates[1], orthodrome_coordinates[2], QColor(0, 255, 0));
}


void MainWindow::on_action_2_triggered()
{

}


void MainWindow::on_action_triggered() // Переход от первого окна ко второму
{
    this->hide();

    sw->show();
}


void MainWindow::on_units_of_loxodrome_measurement_box_currentIndexChanged(int new_index) // Изменение единиц измерения длины локсодромы
{
    ui->loxodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(lox_index, new_index, ui->loxodrome_length_line->text().toDouble())));

    lox_index = new_index;
}


void MainWindow::on_units_of_orthodrome_measurement_box_currentIndexChanged(int new_index) // Изменение единиц измерения длины ортодромы
{
    ui->orthodrome_length_line->setText(QString::number(gf.UnitsOfMeasurementChanging(orth_index, new_index, ui->orthodrome_length_line->text().toDouble())));

    orth_index = new_index;
}

void MainWindow::GetCoordinates(QVector<QVector<double>> coordinates, QColor color) // Построение графика, рассчитанного во втором окне
{
    pf->RePlot(coordinates[0], coordinates[1], coordinates[2], color);
}


void MainWindow::on_reset_camera_clicked()
{
    pf->BackCamera();
}

