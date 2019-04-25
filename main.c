#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

const char* USAGE = "brightness <get|set|adj> [amount]";

const char* BFILE = "/sys/class/backlight/intel_backlight/brightness";
const char* BMAXFILE = "/sys/class/backlight/intel_backlight/max_brightness";

#define BBUFFER_SIZE 10

int read_brightness() {
    FILE* bfile = fopen(BFILE, "r");

    char bbuffer[BBUFFER_SIZE];

    int bread = fread(bbuffer, 1, BBUFFER_SIZE, bfile);
    bbuffer[bread] = 0;

    fclose(bfile);

    return atoi(bbuffer);
}

int read_max_brightness() {
    FILE* bfile = fopen(BMAXFILE, "r");

    char bbuffer[BBUFFER_SIZE];

    int bread = fread(bbuffer, 1, BBUFFER_SIZE, bfile);
    bbuffer[bread] = 0;

    fclose(bfile);

    return atoi(bbuffer);
}

float read_and_calc_perc() {
    int b = read_brightness();
    int mb = read_max_brightness();

    return 100.0f * (float)b / (float)mb;
}

void write_brightness(int val) {
    FILE* bfile = fopen(BFILE, "w");
    if (!bfile) {
        if (errno == EACCES) {
            fprintf(stderr, "Must be run as root!\n");
            exit(1);
        }

        fprintf(stderr, "Failed to open brightness file.\n");
        exit(1);
    }

    fprintf(bfile, "%d", val);

    fclose(bfile);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "%s\n", USAGE);
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        fprintf(stderr, "%s\n", USAGE);
        return 0;
    }

    if (strcmp(argv[1], "get") == 0) {
        printf("%.0f%%\n", read_and_calc_perc());
        return 0;
    }

    if (strcmp(argv[1], "set") == 0) {
        if (argc < 3) {
            fprintf(stderr, "%s\n", USAGE);
            return 1;
        } 

        char* endptr;
        int val = strtol(argv[2], &endptr, 10);

        if (*argv[2] == 0 || *endptr != 0) {
            fprintf(stderr, "%s\n", USAGE);
            return 1;
        }

        int mb = read_max_brightness();

        int b = (int) ((float)val * (float)mb / 100.0f); 

        write_brightness(b);
        return 0;
    }

    if (strcmp(argv[1], "adj") == 0) {
        if (argc < 3) {
            fprintf(stderr, "%s\n", USAGE);
            return 1;
        } 

        char* endptr;
        int val = strtol(argv[2], &endptr, 10);

        if (*argv[2] == 0 || *endptr != 0) {
            fprintf(stderr, "%s\n", USAGE);
            return 1;
        }

        int mb = read_max_brightness();
        int b = read_brightness();
        int db = (int) ((float)val * (float)mb / 100.0f);

        b += db;

        if (b > mb) b = mb;
        if (b < 0) b = 0;        

        write_brightness(b);
        return 0;
    }

    fprintf(stderr, "%s\n", USAGE);
    return 1;
}
