#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int left_pipe[2];
    int right_pipe[2];
    int pid;
    int has_child = 1;
    pipe(right_pipe);

    if (fork() == 0) {
        close(right_pipe[1]);
        left_pipe[0] = right_pipe[0];

        int input;
        int n = -1;
        has_child = 0;
        while (read(left_pipe[0], &input, 1) != 0) {
            if (n == -1) {
                n = input;
                printf("prime %d\n", n);
            } else if (input % n != 0) {
                if (has_child == 0) {
                    pipe(right_pipe);
                    if (fork() == 0) {
                        close(left_pipe[0]);
                        close(right_pipe[1]);
                        left_pipe[0] = right_pipe[0];
                        n = -1;
                    } else {
                        close(right_pipe[0]);
                        has_child = 1;
                        write(right_pipe[1], &input, 1);
                    }
                } else {
                    write(right_pipe[1], &input, 1);
                }
            }
        }
    } else { // init process (generator)
        close(right_pipe[0]);
        for (int i = 2; i <= 35; ++i) {
            write(right_pipe[1], &i, 1);
        }
    }

    if (has_child == 1) {
        close(right_pipe[1]);
        wait(&pid);
    }
    
    exit(0);
}