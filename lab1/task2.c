#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define N (100000 * 2048)
#define NTHRD 1 // amount of threads to create O.o

typedef struct {
        int imin;
        int imax;
        double psum;
} args_t;

double *a;
void *partialsum(void *p) {
        args_t *args = (args_t *)p;
        args->psum = 0.0;
        for (int i = args->imin; i < args->imax; i++)
                args->psum += a[i];
}

int main() {
        pthread_t thrd[NTHRD];
        args_t thrdargs[NTHRD];
        // Create input array
        a = (double *)malloc(N * sizeof(double));
        for (int i = 0; i < N; i++)
                a[i] = i;
	struct timeval start, finish, delta;

        gettimeofday(&start, NULL);
        for (int i = 0; i < 10; i++) {

                // Create threads
                for (int i = 0; i < NTHRD; i++) {
                        thrdargs[i].imin = i * N / NTHRD;
                        thrdargs[i].imax = (i + 1) * N / NTHRD;
                        if (pthread_create(&thrd[i], NULL, partialsum,
                                           &thrdargs[i])) {
                                perror("pthread create");
                                exit(1);
                        }
                }

                // Wait for threads to finish
                for (int i = 0; i < NTHRD; i++)
                        if (pthread_join(thrd[i], NULL)) {
                                perror("pthread join");
                                exit(1);
                        }

                // Compute g l o b a l sum
                double gsum = 0.0;
                for (int i = 0; i < NTHRD; i++)
                        gsum += thrdargs[i].psum;

                //       printf("gsum = %.1f\n", gsum);
        }
        gettimeofday(&finish, NULL);
	delta.tv_sec = finish.tv_sec - start.tv_sec;
	delta.tv_usec = finish.tv_usec - start.tv_usec;
        printf("avg time One thread N = %d: %d s  %d us\n", N, delta.tv_sec, delta.tv_usec);
}
