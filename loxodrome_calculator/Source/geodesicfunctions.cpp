#include "Headers/geodesicfunctions.h"
#include "cmath"
#include "QDebug"

GeodesicFunctions::GeodesicFunctions() {}

double GeodesicFunctions::WestToEast(double longitude) // Перевод з.д. в в.д.
{
    return (-1 * longitude);
}

double GeodesicFunctions::SouthToNorth(double latitude) // Перевод ю.ш. в с.ш.
{
    return (-1 * latitude);
}

double GeodesicFunctions::UnitsOfMeasurementChanging(int previous_index, int current_index, double value) // Переход между км и морскими/сухопутными милями
{
    switch (previous_index * 3 + current_index) { // Рассматриваем как число в троичной с.с.
    case 1:
        return value / 1.852;
        break;
    case 2:
        return value / 1.60934;
        break;
    case 3:
        return value * 1.852;
        break;
    case 4:
        return value / 1.852;
        break;
    case 5:
        return value * 1.852 / 1.60934;
        break;
    case 6:
        return value * 1.60934;
        break;
    case 7:
        return value / 1.852 * 1.60934;
        break;
    default:
        return value;
        break;
    }
}

double GeodesicFunctions::FindCourse(double start_latitude, double end_latitude, double start_longitude, double end_longitude) // Функция для нахождения курса по двум точкам
{
    double course;

    double longitude_difference;
    if (abs(end_longitude - start_longitude) <= M_PI) // Улсовия пля построения меньшей локсодромы
    {
        longitude_difference = (end_longitude - start_longitude);
    }
    if (end_longitude - start_longitude < -M_PI)
    {
        longitude_difference = (2 * M_PI - abs(end_longitude - start_longitude));
    }
    if (end_longitude - start_longitude > M_PI)
    {
        longitude_difference = - (2 * M_PI - abs(end_longitude - start_longitude));
    }

    course = atan2(longitude_difference, log(tan(M_PI/4 + end_latitude/2)) - log(tan(M_PI/4 + start_latitude/2))); // Сама формула
    course = fmod(course + 2*M_PI, 2*M_PI); // Переход к интервалу [0; 2pi]

    if (start_latitude == M_PI/2 && fmod(end_latitude, M_PI/2) != 0) {course = M_PI;} // Условия при полюсах
    if (start_latitude == -M_PI/2 && fmod(end_latitude, M_PI/2) != 0) {course = 0;}
    if (end_latitude == M_PI/2 && fmod(start_latitude, M_PI/2) != 0) {course = 0;}
    if (end_latitude == -M_PI/2 && fmod(start_latitude, M_PI/2) != 0) {course = M_PI;}

    return course;
}

double GeodesicFunctions::FindLoxodromeLength(double start_latitude, double end_latitude, double start_longitude, double end_longitude, double course) // Функция для нахождения длины локсодромы
{
    double loxodrome_length = (end_latitude - start_latitude) / cos(course) * 6371; // Уравнение длины локсодромы (на сфере)

    if (start_latitude == end_latitude) {loxodrome_length = (abs(end_longitude - start_longitude) * cos(start_latitude) * 6371);} // Условие при движении по параллели, то есть совпадении широт

    return loxodrome_length;
}

double GeodesicFunctions::FindOrthodromeLength(double start_latitude, double end_latitude, double start_longitude, double end_longitude) // Функция для нахождения длины ортодромы
{
    double orthodrome_length = 6371 * acos(cos(M_PI/2 - start_latitude) * cos(M_PI/2 - end_latitude) + sin(M_PI/2 - start_latitude) * sin(M_PI/2 - end_latitude) * cos(abs(end_longitude - start_longitude)));

    return orthodrome_length;
}

double GeodesicFunctions::FindLongitude(double start_latitude, double assigned_latitude, double start_longitude, double course) // Нахождения долготы по начальной точке, курсу и конечной широте
{
    double unknown_longitude = tan(course) * (log(tan(M_PI/4 + assigned_latitude/2)) - log(tan(M_PI/4 + start_latitude/2))) + start_longitude; // Уравнение локсодромы по двум точкам

    while ((unknown_longitude < -M_PI) || (unknown_longitude > M_PI)) // Учитываем цикличность
    {
        if (unknown_longitude < -M_PI) {unknown_longitude = unknown_longitude + 2 * M_PI;}

        if (unknown_longitude > M_PI) {unknown_longitude = unknown_longitude - 2 * M_PI;}
    }

    return unknown_longitude;
}

double GeodesicFunctions::FindLatitude(double start_latitude, double start_longitude, double assigned_longitude, double course)
{
    double delta_longitude;
    if (abs(assigned_longitude - start_longitude) <= M_PI) // Улсовия пля построения меньшей дуги большого круга
    {
        delta_longitude = (assigned_longitude - start_longitude);
    }
    if (assigned_longitude - start_longitude < -M_PI)
    {
        delta_longitude = (2 * M_PI - abs(assigned_longitude - start_longitude));
    }
    if (assigned_longitude - start_longitude > M_PI)
    {
        delta_longitude = - (2 * M_PI - abs(assigned_longitude - start_longitude));
    }

    double unknown_latitude = 2 * (atan(exp(delta_longitude / tan(course)) * tan(M_PI / 4 + start_latitude / 2)) - M_PI / 4);
    return unknown_latitude;
}

QVector<QVector<double>> GeodesicFunctions::FindLoxodromePoints(double start_latitude, double end_latitude, double start_longitude, double end_longitude, double course, double length) // Функция для нохождения точек локсодромы
{
    QVector<double> X; // Определяем массивы координат
    QVector<double> Z;
    QVector<double> Y;

    X.append(cos(start_longitude)*cos(start_latitude)); // Записываем первую точку
    Z.append(cos(start_latitude)*sin(start_longitude));
    Y.append(sin(start_latitude));

    // X.append(cos(end_longitude)*cos(end_latitude));
    // Z.append(cos(end_latitude)*sin(end_longitude));
    // Y.append(sin(end_latitude));

    int steps = int(round(length / 250));


    if (start_latitude == end_latitude && start_longitude != end_longitude) // Условие совпадения широт
    {
        double delta_longitude = (end_longitude - start_longitude) / steps;
        double latitude = start_latitude;

        for (int i = 1; i < steps; i++)
        {
            double longitude = start_longitude + i * delta_longitude;

            X.append(cos(longitude)*cos(latitude));
            Z.append(cos(latitude)*sin(longitude));
            Y.append(sin(latitude));
        }
    }

    else if (start_latitude == M_PI/2 || start_latitude == -M_PI/2) // Условие начальной точки на полюсе
    {
        double delta_latitude = (end_latitude - start_latitude) / steps; // Определяем шаг и входим в цикл, где поочередно просчитываем все точки

        for (int i = 1; i < steps; i++)
        {
            double latitude = start_latitude + i * delta_latitude;
            double longitude = end_longitude;

            X.append(cos(longitude)*cos(latitude));
            Z.append(cos(latitude)*sin(longitude));
            Y.append(sin(latitude));
        }
    }

    else // Обычный случай
    {
        double delta_latitude = (end_latitude - start_latitude) / steps; // Определяем шаг и входим в цикл, где поочередно просчитываем все точки

        for (int i = 1; i < steps; i++)
        {
            double latitude = start_latitude + i * delta_latitude;
            double longitude = FindLongitude(start_latitude, latitude, start_longitude, course);

            X.append(cos(longitude)*cos(latitude));
            Z.append(cos(latitude)*sin(longitude));
            Y.append(sin(latitude));
        }
    }

    X.append(cos(end_longitude)*cos(end_latitude)); // Записываем конечную точку
    Z.append(cos(end_latitude)*sin(end_longitude));
    Y.append(sin(end_latitude));

    QVector<QVector<double>> coordinates = {X, Z, Y};

    return coordinates;
}

QVector<QVector<double>> GeodesicFunctions::FindNumericalLoxodromePoints(double start_latitude, double end_latitude, double start_longitude, double end_longitude, double course, double delta_longitude)
{
    QVector<double> X; // Определяем массивы координат
    QVector<double> Z;
    QVector<double> Y;

    X.append(cos(start_longitude)*cos(start_latitude)); // Записываем первую точку
    Z.append(cos(start_latitude)*sin(start_longitude));
    Y.append(sin(start_latitude));

    if (delta_longitude == 0) // Если шаг 0, что будет только начальная точка
    {
        QVector<QVector<double>> coordinates = {X, Z, Y};
        return coordinates;
    }

    if (start_latitude == end_latitude)
    {
        QVector<QVector<double>> coordinates = {X, Z, Y};
        return coordinates;
    }

    // Добавляем вторую точку
    if (sin(course) < 0) {delta_longitude  = - delta_longitude;}
    if (sin(course) > 0) {delta_longitude  = delta_longitude;}


    double latitude;
    double longitude = (floor(start_longitude / (delta_longitude)) + 1) * delta_longitude;
    latitude = M_PI / 2 - atan2(sin(M_PI/2 - start_latitude), (cos(M_PI/2 - start_latitude) * cos(delta_longitude) + sin(delta_longitude) / tan(course))); // проверить

    X.append(cos(longitude)*cos(latitude));
    Z.append(cos(latitude)*sin(longitude));
    Y.append(sin(latitude));

    while (true) // Последовательно добавляем остальные точки
    {

        latitude = M_PI / 2 - atan2(sin(M_PI/2 - latitude), (cos(M_PI/2 - latitude) * cos(delta_longitude) + sin(delta_longitude) / tan(course))); // проверить
        longitude += delta_longitude;

        if (latitude <= end_latitude && cos(course) < 0) {break;}
        if (latitude >= end_latitude && cos(course) > 0) {break;}

        X.append(cos(longitude)*cos(latitude));
        Z.append(cos(latitude)*sin(longitude));
        Y.append(sin(latitude));
    }

    QVector<QVector<double>> coordinates = {X, Z, Y};

    return coordinates;
}


QVector<QVector<double>> GeodesicFunctions::FindOrthodromePoints(double start_latitude, double end_latitude, double start_longitude, double end_longitude, double length) // Функция для нохождения точек ортодромы
{
    QVector<double> X; // Определяем массивы координат
    QVector<double> Z;
    QVector<double> Y;

    X.append(cos(start_longitude)*cos(start_latitude)); // Записываем первую точку
    Z.append(cos(start_latitude)*sin(start_longitude));
    Y.append(sin(start_latitude));

    // X.append(cos(end_longitude)*cos(end_latitude));
    // Z.append(cos(end_latitude)*sin(end_longitude));
    // Y.append(sin(end_latitude));

    int steps = int(round(length / 250));

    if (start_longitude == end_longitude) // Случай совпадения долгот
    {
        double delta_latitude = (end_latitude - start_latitude) / steps;
        double longitude = start_longitude;

        for (int i = 1; i < steps; i ++)
        {
            double latitude = start_latitude + i * delta_latitude;

            X.append(cos(longitude)*cos(latitude));
            Z.append(cos(latitude)*sin(longitude));
            Y.append(sin(latitude));
        }
    }

    else if (start_latitude == M_PI/2 || start_latitude == -M_PI/2) // Если начальная точка на полюсе
    {
        double delta_latitude = (end_latitude - start_latitude) / steps;
        double longitude = end_longitude;

        for (int i = 1; i < steps; i ++)
        {
            double latitude = start_latitude + i * delta_latitude;

            X.append(cos(longitude)*cos(latitude));
            Z.append(cos(latitude)*sin(longitude));
            Y.append(sin(latitude));
        }
    }
    else // Случай обычный
    {
        double delta_longitude;
        if (abs(end_longitude - start_longitude) <= M_PI) // Улсовия пля построения меньшей дуги большого круга
        {
            delta_longitude = (end_longitude - start_longitude) / steps;
        }
        if (end_longitude - start_longitude < -M_PI)
        {
            delta_longitude = (2 * M_PI - abs(end_longitude - start_longitude)) / steps;
        }
        if (end_longitude - start_longitude > M_PI)
        {
            delta_longitude = - (2 * M_PI - abs(end_longitude - start_longitude)) / steps;
        }

        double A = cos(start_latitude) * sin(start_longitude) * sin(end_latitude) - cos(end_latitude) * sin(end_longitude) * sin(start_latitude); // Константы для уравнения плоскости дуги большого круга
        double B = cos(end_latitude) * cos(end_longitude) * sin(start_latitude) - cos(start_latitude) * cos(start_longitude) * sin(end_latitude);
        double C = cos(start_latitude) * cos(end_latitude) * sin(end_longitude - start_longitude);

        for (int i = 1; i < steps; i ++)
        {
            double longitude = start_longitude + i * delta_longitude;
            double latitude = - atan((A * cos(longitude) + B * sin(longitude)) / C); // Само уравнение

            X.append(cos(longitude)*cos(latitude));
            Z.append(cos(latitude)*sin(longitude));
            Y.append(sin(latitude));
        }
    }

    X.append(cos(end_longitude)*cos(end_latitude)); //Записываем конечную точку
    Z.append(cos(end_latitude)*sin(end_longitude));
    Y.append(sin(end_latitude));

    QVector<QVector<double>> coordinates = {X, Z, Y};

    return coordinates;
}
QVector<QVector<double>> GeodesicFunctions::FindEcuatorPoints() // Считаем точки экватора
{
    QVector<double> X; // Определяем массивы координат
    QVector<double> Z;
    QVector<double> Y;

    for (double i = 0; i < 360; i += 10)
    {
        double longitude = i * M_PI / 180;

        X.append(cos(longitude));
        Z.append(sin(longitude));
        Y.append(0);
    }

    QVector<QVector<double>> coordinates = {X, Z, Y};

    return coordinates;
}

QVector<QVector<double>> GeodesicFunctions::FindGreenwichPoints() // Считаем точки Гринвичского меридиана
{
    QVector<double> X_0; // Определяем массивы координат Гринвичского меридиана
    QVector<double> Z_0;
    QVector<double> Y_0;

    QVector<double> X_180; // Определяем массивы координат меридиана противоположного Гринвичскому и двух ортогональных ему
    QVector<double> Z_180;
    QVector<double> Y_180;

    for (double i = -90; i < 90; i += 10) // Считаем Гринвич
    {
        double latitude = i * M_PI / 180;

        X_0.append(cos(latitude));
        Z_0.append(0);
        Y_0.append(sin(latitude));
    }

    for (double i = 90; i < 270; i += 10) // Считаем обратный
    {
        double latitude = i * M_PI / 180;

        X_180.append(cos(latitude));
        Z_180.append(0);
        Y_180.append(sin(latitude));
    }

    for (double i = 0; i < 360; i += 10) // Считаем ортогональные
    {
        double latitude = i * M_PI / 180;

        X_180.append(0);
        Z_180.append(cos(latitude));
        Y_180.append(sin(latitude));
    }

    QVector<QVector<double>> coordinates = {X_0, Z_0, Y_0, X_180, Z_180, Y_180};

    return coordinates;
}

QVector<QVector<double>> GeodesicFunctions::GetLatitudeAndLongitudeFromXZY(QVector<QVector<double>> coordinates)
{
    QVector<double> X = coordinates[0];
    QVector<double> Z = coordinates[1];
    QVector<double> Y = coordinates[2];

    QVector<double> latitudes;
    QVector<double> longitudes;

    for (int i = 0; i < X.length(); i++)
    {
        latitudes.append(asin(Y[i]));

        longitudes.append(atan2(Z[i], X[i]));
    }

    QVector<QVector<double>> spheric_coordinates = {longitudes, latitudes};

    return spheric_coordinates;
}
