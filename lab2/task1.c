#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 8
#define PI 3.141592
#define AREA_SQUARE 4.0

double N_try;
double N_hit;

pthread_mutex_t lock;
pthread_t threads[NUM_THREADS];

int check_if_hit(double x, double y)
{
    if((x*x + y*y) <= 1.0){
        return 1;
    }
    return 0;
}

double calculate_estimate(){
        if(N_hit == 0){
                return 0;
        }
        double res = ((N_hit / N_try) * AREA_SQUARE);
        printf("res: %f\n", res);
        return res; 
}

void *placeholderfunc(){
        struct drand48_data rndBuffer;
        srand48_r(pthread_self(), &rndBuffer);          
        while(2)
        {
                double x;
                drand48_r(&rndBuffer, &x);
                double y;
                drand48_r(&rndBuffer, &y);
                //printf("%f, %f, %f\n", x, y, x/y);
                int hit = check_if_hit(x, y);
                pthread_mutex_lock(&lock);
                N_hit += hit;
                N_try++;
                pthread_mutex_unlock(&lock);
        }
        

}

int main(int argc, char const *argv[]){
    
        N_try = 0.0;
        N_hit = 0.0;
        pthread_mutex_init(&lock, NULL);
        for (int i = 0; i < NUM_THREADS; i++)
        {
                pthread_create( &threads[i], NULL, &placeholderfunc, NULL );
        }
        while(1){
                printf("num hits: %f, number of tries: %f\n",N_hit,N_try);
                double est = calculate_estimate();
                printf("approximate area \t %f\n", est);
                sleep(2);
        
        }
        
        return 0;
}
