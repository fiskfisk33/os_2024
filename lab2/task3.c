#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <sys/time.h>


//binary semaphore only has values 0 or 1

int counter = 0;
const int nthr = 1000;
typedef struct {
    volatile atomic_flag flag;
    // MEmbers to be defined
} mysemaphore_t;

mysemaphore_t sem;
struct timespec threadtimes[1000];

int sem_init(mysemaphore_t *s){
    atomic_flag_clear(&(s->flag));
    // sets the state to false (0) 
    return 0;
}

int sem_wait(mysemaphore_t *s){
        //int delay = 1;
        while (atomic_flag_test_and_set(&(s->flag)))
        {
                //usleep(delay);
                //delay *= 2;
        }
    return 0;
}

int sem_post(mysemaphore_t *s){
    atomic_flag_clear(&(s->flag));
    return 0;
}

void* func(void *arg) {
    sleep(1);
        struct timespec *threadtime = (struct timespec*) arg;
        struct timespec start, finish, delta;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);


        sem_wait(&sem);
        counter++;
        sem_post(&sem);


        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);

        delta.tv_sec = finish.tv_sec - start.tv_sec;
	    delta.tv_nsec = finish.tv_nsec - start.tv_nsec;
        if(delta.tv_nsec < 0){
            delta.tv_sec --;
            delta.tv_nsec += 1000000000;
        }

        threadtime->tv_sec = delta.tv_sec;
        threadtime->tv_nsec = delta.tv_nsec;


}

int add_timespecs(struct timespec *t1, struct timespec *t2){
    struct timespec ans;
    t1->tv_sec += t2->tv_sec;
    t1->tv_nsec += t2->tv_nsec;
    if (t1->tv_nsec > 1000000000){
        t1->tv_sec ++;
        t1->tv_nsec -= 1000000000;
    }
    return 1;
}
int main(){
        pthread_t thr[nthr];
        sem_init(&sem);
        
        for(int i = 0; i < nthr; i++)
                pthread_create(&thr[i], NULL, &func, &threadtimes[i]);
        for (int i = 0; i < nthr; i++)
                pthread_join(thr[i], NULL);
        struct timespec totaltime;
        totaltime.tv_nsec = 0;
        totaltime.tv_sec = 0;
        for (int i = 0; i < nthr; i++)
        {
            add_timespecs(&totaltime, &threadtimes[i]);
        }
        

        printf("programtid: %d,%09d s\n", totaltime.tv_sec, totaltime.tv_nsec);
        printf("counter = %d\n", counter);
        return 0;
}

