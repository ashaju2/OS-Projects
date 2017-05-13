/* README
Compile:
    gcc main.cpp -lstdc++ -lpthread -Wno-write-strings -o main
execute:
    ./main [Max weight of bridge] < [Input File]  
*/

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <queue>
#include <fstream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

queue <string> readIn;

int maxweight;
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
    
    if(threadarg->weight > maxweight){
    cout << "Vehicle " << threadarg->vID << " exceeds maximum bridge load." << endl;
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);

    }
    
        while ((bridgeLoad + threadarg->weight) > maxweight) {
            pthread_cond_wait(&empty, &mutex);
        }

        bridgeLoad += threadarg->weight;
        cout << "Vehicle " << threadarg->vID << " goes on bridge." << endl; 
        cout << "The current bridge load is " << bridgeLoad << " tons." << endl;
        if(bridgeLoad < maxweight){
            pthread_cond_signal(&empty);
            }
    
}

void leaveBridge(vehicle * threadarg){
        bridgeLoad -= threadarg->weight;
        cout << "Vehicle " << threadarg->vID << " leaves the bridge." << endl;
        cout << "The current bridge load is " << bridgeLoad << " tons." << endl;
        pthread_cond_signal(&empty);
}

void *thread_Run(void *threadarg) {

   struct vehicle *my_data;
   my_data = (struct vehicle *) threadarg;

   pthread_mutex_lock(&mutex);
   cout << "Vehicle " << my_data->vID << " arrives at bridge." << endl;
   cout << "The current bridge load is " << bridgeLoad << " tons." << endl;

   enterBridge((vehicle *) threadarg);
   pthread_mutex_unlock(&mutex);
   sleep(my_data->cross_time);
   pthread_mutex_lock(&mutex);
   leaveBridge((vehicle *) threadarg);
   pthread_mutex_unlock(&mutex);

   pthread_exit(NULL);
}

//---------------------------------------------------------------------------------------------

int main(int argc, char **argv){
	string input;
    string vehicleID;
    int arrive;
    int cross;
	int  w;
	int j = 0;

	while(getline(cin, input)) {
		readIn.push(input);
	}
	int const NUM_THREADS = readIn.size();
    maxweight = atoi(argv[1]);

//------------------------------------------------------------------------------------------------

   cout << "Maximum bridge load is " << maxweight << " tons." << endl;
   pthread_mutex_init(&mutex, NULL);

   pthread_t threads[NUM_THREADS];
   struct vehicle td[NUM_THREADS];
	
    int rc;
   int i;
   int threadCount = 0;


	while (!readIn.empty())
        {
            istringstream iss(readIn.front());
            iss >> vehicleID;
            iss >> arrive;
			iss >> w;
            iss >> cross;

			td[i].vID = vehicleID;
			td[i].arrive_delay = arrive;
			td[i].weight = w;
			td[i].cross_time = cross;
            readIn.pop();

      sleep(td[i].arrive_delay);
      rc = pthread_create(&threads[i], NULL, thread_Run, (void *)&td[i]);
	
      if (rc){
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
			i++;
            threadCount++;
   }

//------------------------------------------------------------------------------------------------------
   
    for( i=0; i < threadCount; i++ ){
        	pthread_join(threads[i], NULL);
    }

	cout << endl << "Total number of vehicles: " << threadCount << endl;

    return 0;
}