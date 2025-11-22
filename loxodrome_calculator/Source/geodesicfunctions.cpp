#include "Headers/geodesicfunctions.h"
#include "cmath"
#include "QDebug"

GeodesicFunctions::GeodesicFunctions() {}

double GeodesicFunctions::WestToEast(double longitude)
{
    return (-1 * longitude);
}

double GeodesicFunctions::SouthToNorth(double latitude)
{
    return (-1 * latitude);
}

double GeodesicFunctions::UnitsOfMeasurementChanging(int previous_index, int current_index, double value)
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

double GeodesicFunctions::FindCourse(double start_latitude, double end_latitude, double start_longitude, double end_longitude)
{
    double course;

    course = atan2(end_longitude - start_longitude, log(tan(M_PI/4 + end_latitude/2)) - log(tan(M_PI/4 + start_latitude/2)));
    course = fmod(course + 2*M_PI, 2*M_PI);

    if (start_latitude == M_PI/2 && fmod(end_latitude, M_PI/2) != 0) {course = M_PI;} // Условия при полюсах
    if (start_latitude == -M_PI/2 && fmod(end_latitude, M_PI/2) != 0) {course = 0;}
    if (end_latitude == M_PI/2 && fmod(start_latitude, M_PI/2) != 0) {course = 0;}
    if (end_latitude == -M_PI/2 && fmod(start_latitude, M_PI/2) != 0) {course = M_PI;}

    return course;
}

double GeodesicFunctions::FindLoxodromeLength(double start_latitude, double end_latitude, double start_longitude, double end_longitude, double course)
{
    double loxodrome_length = (end_latitude - start_latitude) / cos(course) * 6371; // Уравнение длины локсодромы (на сфере)

    if (start_latitude == end_latitude) {loxodrome_length = (abs(end_longitude - start_longitude) * cos(start_latitude) * 6371);} // Условие при движении по параллели, то есть совпадении широт

    return loxodrome_length;
}

double GeodesicFunctions::FindOrthodromeLength(double start_latitude, double end_latitude, double start_longitude, double end_longitude)
{
    double orthodrome_length = 6371 * acos(cos(M_PI/2 - start_latitude) * cos(M_PI/2 - end_latitude) + sin(M_PI/2 - start_latitude) * sin(M_PI/2 - end_latitude) * cos(abs(end_longitude - start_longitude)));

    return orthodrome_length;
}

double GeodesicFunctions::FindLongitude(double start_latitude, double assigned_latitude, double start_longitude, double course)
{
    double unknown_longitude = tan(course) * (log(tan(M_PI/4 + assigned_latitude/2)) - log(tan(M_PI/4 + start_latitude/2))) + start_longitude; // Уравнение локсодромы по двум точкам

    while ((unknown_longitude < -M_PI) || (unknown_longitude > M_PI)) // Учитываем цикличность
    {
        if (unknown_longitude < -M_PI) {unknown_longitude = unknown_longitude + 2 * M_PI;}

        if (unknown_longitude > M_PI) {unknown_longitude = unknown_longitude - 2 * M_PI;}
    }

    return unknown_longitude;
}
