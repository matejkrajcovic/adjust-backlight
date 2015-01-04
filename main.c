#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "sun.h"

#define LATITUDE 48.15349638
#define LONGITUDE 17.11362930
#define LOCAL_OFFSET 1

bool before(time_t first, time_t second);

bool after(time_t first, time_t second);

double get_max_brightness(void);

void set_brightness(double brightness);

int main(void)
{
	time_t now = time(NULL);

	struct tm *today = localtime(&now);
	int day = today->tm_mday;
	int month = today->tm_mon + 1;
	int year = today->tm_year + 1900;
	
	struct tm *sunrise = calculate_sunrise(false, day, month, year, LATITUDE, LONGITUDE, LOCAL_OFFSET);
	struct tm *sunset  = calculate_sunrise( true, day, month, year, LATITUDE, LONGITUDE, LOCAL_OFFSET);
	
	/* change brightness before sunrise or after sunset */
	if (before(now, mktime(sunrise)) || after(now, mktime(sunset))) {
		double max_brightness = get_max_brightness();
		set_brightness(max_brightness / 2);
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
	FILE *f = fopen("/sys/class/backlight/acpi_video0/max_brightness", "r");
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
	FILE *f = fopen("/sys/class/backlight/acpi_video0/brightness", "w");
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
