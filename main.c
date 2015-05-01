#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "sun.h"


#define LATITUDE 48.15349638
#define LONGITUDE 17.11362930
#define LOCAL_OFFSET 1
#define SECONDS_IN_HOUR 3600

#define NIGHT_DIM_RATIO 0.25
#define TWILIGHT_DIM_RATIO 0.5

bool before(time_t first, time_t second);

bool after(time_t first, time_t second);

double get_max_brightness(void);

void set_brightness(double brightness);

int main(void)
{
	if (getuid() != 0) {
		fprintf(stderr, "Run as root.\n");
		exit(EXIT_FAILURE);
	}

	time_t now = time(NULL);

	struct tm *today = localtime(&now);
	int day = today->tm_mday;
	int month = today->tm_mon + 1;
	int year = today->tm_year + 1900;

	struct tm *sunrise = calculate_sunrise(false, day, month, year, LATITUDE, LONGITUDE, LOCAL_OFFSET);
	struct tm *sunset  = calculate_sunrise( true, day, month, year, LATITUDE, LONGITUDE, LOCAL_OFFSET);

	time_t sunrise_time_t = mktime(sunrise);
	time_t sunset_time_t = mktime(sunset);

	/* before sunrise or after sunset */
	bool night = before(now, sunrise_time_t) || after(now, sunset_time_t);

	/* hour after sunrise or hour before sunset */
	bool twilight = before(now, sunrise_time_t + SECONDS_IN_HOUR) || after(now, sunset_time_t - SECONDS_IN_HOUR);

	double max_brightness = get_max_brightness();

	if (night) {
		set_brightness(max_brightness * NIGHT_DIM_RATIO);
	} else if (twilight) {
		set_brightness(max_brightness * TWILIGHT_DIM_RATIO);
	}

	free(sunset);
	free(sunrise);
	return EXIT_SUCCESS;
}

/* first is before second */
bool before(time_t first, time_t second)
{
	return (difftime(first, second) > 0);
}

/* first is after second */
bool after(time_t first, time_t second)
{
	return (difftime(first, second) < 0);
}

double get_max_brightness(void)
{
	double max_brightness;
	FILE *f = fopen("/sys/class/backlight/radeon_bl0/max_brightness", "r");
	if (f == NULL) {
		perror("get_max_brightness fopen()");
		exit(EXIT_FAILURE);
	}

	if (fscanf(f, "%lf", &max_brightness) != 1) {
		perror("get_max_brightness fscanf()");
		if (fclose(f) != 0) {
			perror("get_max_brightness fclose()");
		}
		exit(EXIT_FAILURE);
	}

	if (fclose(f) != 0) {
		perror("get_max_brightness fclose()");
		exit(EXIT_FAILURE);
	}

	return max_brightness;
}

void set_brightness(double brightness)
{
	FILE *f = fopen("/sys/class/backlight/radeon_bl0/brightness", "w");
	if (f == NULL) {
		perror("set_brightness fopen()");
		exit(EXIT_FAILURE);
	}

	if (fprintf(f, "%.0f", brightness) < 0) {
		perror("set_brightness fprintf()");
		if (fclose(f) != 0) {
			perror("set_brightness fclose()");
		}
		exit(EXIT_FAILURE);
	}

	if (fclose(f) != 0) {
		perror("set_brightness fclose()");
	}

	return;
}
