#ifndef SUN_H
#define SUN_H
#include <stdbool.h>
#include <time.h>

/* Utilities */
double to_degree(double radian);

double to_radian(double degree);

double adjust_to_range(double value, double min, double max);

/* Main functions  */
int calculate_day_of_year(int day, int month, int year);

struct tm *calculate_sunrise(bool sunrise, int day, int month, int year, double latitude, double longitude, double local_offset);
#endif /* SUN_H */
