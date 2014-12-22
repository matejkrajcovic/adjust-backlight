#include <stdio.h>
#include <stdlib.h>

double get_max_brightness(void);

void set_brightness(double brightness);

int main(void)
{
	double max_brightness = get_max_brightness();
	set_brightness(max_brightness / 2);
	return EXIT_SUCCESS;
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
