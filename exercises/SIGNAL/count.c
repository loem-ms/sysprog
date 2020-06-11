#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* atoi */
#include <unistd.h> /* sleep, usleep */
#include <time.h>   /* nanosleep, clock_nanosleep */

#define EMOJI_ZOMBIE "\xF0\x9F\xA7\x9F"
#define PLAIN_ZOMBIE "Z..."

int sleep_halfsec(void) {
    // sleep(1);
    // usleep(500 * 1000);
    struct timespec ts = { 0, 500 * 1000 * 1000 };
#ifdef __linux
    // WSL1 issue: new glibc uses CLOCK_REALTIME although WSL1 doesn't implement it
    // @see https://qiita.com/mmns/items/eaf42dd3345a2285ff9e
    return clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
#else
    return nanosleep(&ts, NULL);
#endif
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <num> [label]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int sec = atoi(argv[1]);
    for (int i = sec; i > 0; i--) {
        if (i <= 3) {
            if (argc > 2) {
                fprintf(stderr, "%s-%d ", argv[2], i);
            } else {
                fprintf(stderr, "%d ", i);
            }
        }
        sleep_halfsec();
    }
    int use_emoji = getenv("NO_EMOJI") == NULL;
    fprintf(stderr, "%s ", use_emoji ? EMOJI_ZOMBIE : PLAIN_ZOMBIE);
    return 0;
}
