#include <signal.h> /* signal */
#include <unistd.h> /* write */


int main() {
    /* Exercise 1: Implement here. */
    signal(SIGINT,SIG_IGN);
    /* Exercise 2: Implement here. */
    // alarm(5);

    /* Prints dots. */
    for (;;) {
        for (int j = 0; j < 1024 * 1024 * 500; j++)
            ; /* busy loop */
        write(STDERR_FILENO, ".", 1);
    }
    return 0;
}
