#include "sun.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define PI 3.14159265358979323846

double to_degree(double radian)
{
	return (180 / PI) * radian;
}

double to_radian(double degree)
{
	return (PI / 180) * degree;
}

/*
 * Adjust to range [min, max).
 */
double adjust_to_range(double value, double min, double max)
{
	double range = max - min;

	if (value < min) {
		double needed_additions = ceil(abs(min - value) / range);
		return value + (range * needed_additions);
	} else if (value >= max) {
		double needed_substractions = ceil(abs(value - max) / range);
		return value - (range * needed_substractions);
	} else {
		return value;
	}
}

int calculate_day_of_year(int day, int month, int year)
{
        int n1 = (int) floor(275.0 * (double) month / 9.0);
        int n2 = (int) floor(((double) month + 9.0) / 12.0);
        int n3 = (int) (1.0 + floor(((double) year - 4.0 * floor((double) year / 4.0) + 2.0) / 3.0));
        int n = (int) n1 - (n2 * n3) + day - 30.0;

        return n;
}

/*
 * Algorithm from http://williams.best.vwh.net/sunrise_sunset_algorithm.htm
 * Inspiration from https://gist.github.com/Tafkas/4742250/
 * sunrise == true -> sunrise
 * sunrise == false -> sunset
 * */
struct tm *calculate_sunrise(bool sunrise, int day, int month, int year, double latitude, double longitude, double local_offset)
{
	int day_of_year = calculate_day_of_year(day, month, year);
	double zenith = 90.83333333333333;

	/* convert the longitude to hour value and calculate an approximate time */
	double lngHour = longitude / 15;
	double t;
	if (sunrise) {
		t = day_of_year + (( 6 - lngHour) / 24);
	} else {
		t = day_of_year + ((18 - lngHour) / 24);
	}
	
	/* calculate the Sun's mean anomaly */
	double M = (0.9856 * t) - 3.289;

	/* calculate the Sun's true longitude */
	double L = M + (1.916 * sin(to_radian(M))) + (0.020 * sin(to_radian(2 * M))) + 282.634;
	L = adjust_to_range(L, 0, 360);

	/* calculate the Sun's right ascension */
	double RA = to_degree(atan(0.91764 * tan(to_radian(L))));
	RA = adjust_to_range(RA, 0, 360);

	/* right ascension value needs to be in the same quadrant as L */
	double Lquadrant  = (floor( L / 90)) * 90;
	double RAquadrant = (floor(RA / 90)) * 90;
	RA = RA + (Lquadrant - RAquadrant);
	
	/* right ascension value needs to be converted into hours */
	RA = RA / 15;

	/* calculate the Sun's declination */
	double sinDec = 0.39782 * sin(to_radian(L));
	double cosDec = cos(asin(sinDec));

	/* calculate the Sun's local hour angle */
	double cosH = (cos(to_radian(zenith)) - (sinDec * sin(to_radian(latitude)))) / (cosDec * cos(to_radian(latitude)));

	/* no sunrise or no sunset */
	if ((cosH > 1) && sunrise) {
		return NULL;
	} else if ((cosH < -1) && !sunrise) {
		return NULL;
	}

	/* finish calculating H and convert into hours */
	double H;
	if (sunrise) {
		H = 360 - to_degree(acos(cosH));
	} else {
		H = to_degree(acos(cosH));
	}
	H = H / 15;

	/* calculate local mean time of rising/setting */
	double T = H + RA - (0.06571 * t) - 6.622;
	
	/* adjust back to UTC */
	double UTC = T - lngHour;
	UTC = adjust_to_range(UTC, 0, 24);

	/* convert UTC value to local time zone of latitude/longitude */
	double localT = UTC + local_offset;

	/* localT is in this form: 12.34567
	 * hour: floor(12.34567) = 12
	 * minutes: floor(0.34567 * 60) = floor(20.7402) = 20
	 * seconds: floor(0.7402 * 60) = floor(44.412) = 44
	 */

	struct tm *time = calloc(1, sizeof(struct tm));
	time->tm_mday = day;
	time->tm_mon = month - 1; /* January is 0 */
	time->tm_year = year - 1900; /* Saved as years from 1900 */

	time->tm_hour = floor(localT);

	localT -= time->tm_hour;
	localT *= 60;
	time->tm_min = floor(localT);

	localT -= time->tm_min;
	localT *= 60;
	time->tm_sec = floor(localT);

	return time;
}
