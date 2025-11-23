#ifndef GEODESICFUNCTIONS_H
#define GEODESICFUNCTIONS_H
#include "QVector"

class GeodesicFunctions
{
public:
    GeodesicFunctions();

    double WestToEast(double);

    double SouthToNorth(double);

    double UnitsOfMeasurementChanging(int, int, double);

    double FindCourse(double, double, double, double);

    double FindLoxodromeLength(double, double, double, double, double);

    double FindOrthodromeLength(double, double, double, double);

    double FindLongitude(double, double, double, double);

    QVector<QVector<double>> FindLoxodromePoints(double, double, double, double, double);

    QVector<QVector<double>> FindOrthodromePoints(double, double, double, double);

    QVector<QVector<double>> FindEcuatorPoints();

    QVector<QVector<double>> FindGreenwichPoints();
};


#endif // GEODESICFUNCTIONS_H
