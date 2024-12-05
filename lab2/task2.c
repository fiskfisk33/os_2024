#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdatomic.h>

//binary semaphore only has values 0 or 1

int counter = 0;
const int nthr = 1000;
typedef struct {
    volatile atomic_flag flag;
    // MEmbers to be defined
} mysemaphore_t;

mysemaphore_t sem;

int sem_init(mysemaphore_t *s){
    atomic_flag_clear(&(s->flag));
    // sets the state to false (0) 
    return 0;
}

int sem_wait(mysemaphore_t *s){
        while (atomic_flag_test_and_set(&(s->flag)))
        {
                /* kbk kör bara kör */
        }
    // Function to be implemented
    return 0;
}

int sem_post(mysemaphore_t *s){
    atomic_flag_clear(&(s->flag));
    return 0;
}

void* func() {
    sleep(1);
    sem_wait(&sem);
    counter++;
    sem_post(&sem);
}
int main(){
        pthread_t thr[nthr];
        sem_init(&sem);
        for(int i = 0; i < nthr; i++)
                pthread_create(&thr[i], NULL, &func, NULL);
        for (int i = 0; i < nthr; i++)
                pthread_join(thr[i], NULL);
        printf("counter = %d\n", counter);
        return 0;
}