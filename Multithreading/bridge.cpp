/*

Sanford Alexander
OS Assignment 3
May 1, 2017
PSID - 1237998

bridge.cpp

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

queue <string> vehiclequeue;

int maxweight;
static int bridgeLoad = 0;
static pthread_mutex_t thread;
static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;

struct vehicleinfo {
	string vehicle_ID;
	int arrival_delay;
	int total_weight;
	int crossing_time;
};

void enterBridge(vehicleinfo *arg) {
	pthread_mutex_lock(&thread);
	
	if (arg->total_weight > maxweight) {
		cout << "Vehicle " << arg->vehicle_ID << " exceeds maximum bridge load." << endl;

		pthread_mutex_unlock(&thread);
		pthread_exit((void*) 0);
	}
	
	while ((arg->total_weight + bridgeLoad) > maxweight) {
		pthread_cond_wait(&ok, &thread);
	}

	cout << "Vehicle " << arg->vehicle_ID << " goes on bridge." << endl;
	
	bridgeLoad = bridgeLoad + arg->total_weight;

	cout << "The current bridge load is " << bridgeLoad << " tons." << endl;

	if (bridgeLoad < maxweight) {
		pthread_cond_signal(&ok);
	}

	pthread_mutex_unlock(&thread);
}

void leaveBridge(vehicleinfo *arg) {
	pthread_mutex_lock(&thread);

	cout << "Vehicle " << arg->vehicle_ID << " leaves the bridge." << endl;

	bridgeLoad = bridgeLoad - arg->total_weight;

	cout << "The current bridge load is " << bridgeLoad << " tons." << endl;

	pthread_cond_signal(&ok);
	pthread_mutex_unlock(&thread);
}

void *vehicle(void *arg) {
	struct vehicleinfo *serial;
	serial = (struct vehicleinfo *) arg;

	cout << "Vehicle " << serial->vehicle_ID << " arrives at bridge." << endl;
	cout << "The current bridge load is " << bridgeLoad << " tons." << endl;

	
	enterBridge((vehicleinfo *) arg);

	sleep(serial->crossing_time);

	leaveBridge((vehicleinfo *) arg);
	
	pthread_exit((void*) 0);
}

int main(int argc, char **argv) {

	string testfile;
	string vehicleIDs;
	int arrivaldelays;
	int totalweights;
	int crossingtimes;
	int i=0;
	int nchildren = 0;

	while (getline (cin, testfile)) {
		vehiclequeue.push(testfile);
	}

	int const maxchildren = vehiclequeue.size();
	maxweight = atoi(argv[1]);

	cout << "Maximum bridge load is " << maxweight << " tons." << endl;

	pthread_mutex_init(&thread, NULL);
	pthread_t child[maxchildren];

	struct vehicleinfo n[maxchildren];
 
	while (!vehiclequeue.empty()) {
		istringstream iss(vehiclequeue.front());
		iss >> vehicleIDs;
		iss >> arrivaldelays;
		iss >> totalweights;
		iss >> crossingtimes;

		n[i].vehicle_ID = vehicleIDs;
		n[i].arrival_delay = arrivaldelays;
		n[i].total_weight = totalweights;
		n[i].crossing_time = crossingtimes;

		vehiclequeue.pop();

		sleep(n[i].arrival_delay);
		pthread_create(&child[i], NULL, vehicle, (void *)&n[i]);

		i++;
		nchildren++;
	}

	for (i = 0; i < nchildren; i++) {
		pthread_join(child[i], NULL);
	}

	cout << endl;
	cout << "Total number of vehicles: " << nchildren << endl;

	return 0;
}