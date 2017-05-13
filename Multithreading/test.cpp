#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUM_THREADS     3

const int maxweight = 10;
static int bridgeLoad = 0;
static pthread_mutex_t mutex;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
    
struct vehicle {
    string vID;
    int arrive_delay;
    int weight;
    int cross_time;
}; 

void enterBridge(vehicle *threadarg){
    pthread_mutex_lock(&mutex);
    while ((bridgeLoad + threadarg->weight) > maxweight) {
        pthread_cond_wait(&empty, &mutex);
        
    }
    bridgeLoad += threadarg->weight;
    cout << "Vehicle " << threadarg->vID << " goes on bridge." << endl; 
    cout << "The current bridge load is " << bridgeLoad << " ton(s)." << endl;
    sleep(threadarg->cross_time);
    pthread_mutex_unlock(&mutex);
    
}

void leaveBridge(vehicle * threadarg){
    pthread_mutex_lock(&mutex);
    bridgeLoad = bridgeLoad - threadarg->weight;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    cout << "Vehicle " << threadarg->vID << " leaves the bridge." << endl;
    
}

void *thread_Run(void *threadarg) {

   struct vehicle *my_data;

   my_data = (struct vehicle *) threadarg;

   cout << "Vehicle " << my_data->vID << " arrives at bridge" << endl;
   cout << "The current bridge load is " << bridgeLoad << " tons." << endl;
   enterBridge((vehicle *) threadarg);
   leaveBridge((vehicle *) threadarg);

   pthread_exit(NULL);
}

int main () {
   cout << "Maximum bridge load is " << maxweight << " tons." << endl;
   pthread_mutex_init(&mutex, NULL);

   pthread_t threads[NUM_THREADS];
   struct vehicle td[NUM_THREADS];
	
   int rc;
   int i;

    td[0].vID = "HIOFCR";
    td[0].arrive_delay = 0;
    td[0].weight = 1;
    td[0].cross_time = 1;

    td[1].vID = "STOL3N";
    td[1].arrive_delay = 3;
    td[1].weight = 10;
    td[1].cross_time = 2;

    td[2].vID = "SHKSPR";
    td[2].arrive_delay = 8;
    td[2].weight = 2;
    td[2].cross_time = 3;


   for( i=0; i < NUM_THREADS; i++ ){
      rc = pthread_create(&threads[i], NULL, thread_Run, (void *)&td[i]);
      
		
      if (rc){
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }
    for( i=0; i < NUM_THREADS; i++ ){
        
            pthread_join(threads[i], NULL);
        
    }
   pthread_exit(NULL);
}