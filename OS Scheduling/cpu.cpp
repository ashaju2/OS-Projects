/*Name: Alen Shaju
*ID: 1434006
*Compiling Instructions:
*          # gcc cpu.cpp -lstdc++ -o cpu
*          # ./cpu < input
*I wasn't able to implement the zero DISK.
*
*/

#include <iostream>
#include <queue>
#include <cstdlib>
#include <string>
#include <sstream>

using namespace std;

double currentTime = 0;
int temp_store = 0;
int tempUser_store = 0;
int temp_core = 0;
int crt;

enum processStatus{ready, running, terminated, blocked};
struct process{
    int processID;
    string processName;
    int processTime;
    int processStartTime;
    int processEndTime;
    processStatus pstat;

};

enum coreStatus{occupied, empty};
struct core{
    int totalTime;
    int coreNum;
    int coreStartTime;
    int coreEndTime;
    coreStatus cstat;
    
};

int processTime;
int processID = 0;
int coreNum;
int slice;


queue< queue<process> > readyQueue;
queue< queue<process> > diskQueue;
queue< queue<process> > input;
queue< queue<process> > display;
queue<string> readIn;
queue<process> processTable;
core ncore[50];
process pStatus;
process ptime;
core coretime;


void fillReadyQueue(){
        readyQueue.push(processTable);
        pStatus.pstat = ready;
}    

void NCORES(int coreNum){
    for(int i = 0; i < coreNum; i++){
        ncore[coreNum].cstat = empty;
        ncore[coreNum].totalTime = 0;
    }
}

void fileInput(){
    string input;
    string processName;
    process item;
    while(getline(cin, input)) {
        readIn.push(input);
    }
    while (!readIn.empty())
        {
            istringstream iss(readIn.front());
            iss >> processName;
            iss >> processTime;
            if (processName == "NCORES"){
                coreNum = processTime;
                NCORES(coreNum);
            }

            else if(processName == "SLICE"){
                slice = processTime;
            }
            else {
                
                if(!processTable.empty() && processName == "NEW"){
                    cout << "Process-Table size : " << processTable.size() <<endl;
                    fillReadyQueue();
                    while(!processTable.empty()){
                            processTable.pop();
                        }
                    }

                if(processName == "NEW"){
                    item.processID = processID;
                    item.processName = processName;
                    item.processTime = processTime;
                    processID++;
                    
                }
                else{
                    item.processName = processName;
                    item.processTime = processTime;
                    }
                processTable.push(item);
               }
         
            readIn.pop();

            if(readIn.empty()){
                cout << "Process-Table size : "<<processTable.size() <<endl;
                fillReadyQueue();
                }
        }
        cout << "Ready-Queue size: "<<readyQueue.size() <<endl;
}

void DISK(){
    int drt = diskQueue.front().front().processTime;
    if(readyQueue.front().front().processName == "CORE" && (readyQueue.front().front().processTime < drt)){
        currentTime = currentTime + drt;
    }
    cout << "Current Time at Disk: " << currentTime << endl;
    cout << "This is the disk queue " << diskQueue.front().front().processName << " " << diskQueue.front().front().processTime << endl;
    temp_store = diskQueue.front().front().processTime;
    diskQueue.front().pop();
    readyQueue.push(diskQueue.front());
    diskQueue.pop();    

}

void DISPLAY(){
    int newDisplayTime = 0;
    pStatus.pstat = blocked;
    int display_Time = display.front().front().processTime;
    if(readyQueue.front().front().processName == "CORE" && (readyQueue.front().front().processTime < display_Time)){
        currentTime = currentTime + display_Time;
        if(tempUser_store > crt){
            currentTime = currentTime - tempUser_store;
            
        }
    }
    else if(tempUser_store > crt){
        currentTime = currentTime - tempUser_store + display_Time;
    }

    if(((display_Time + crt) > tempUser_store) && (tempUser_store!= 0)){
            currentTime = currentTime - ((display_Time + crt) - tempUser_store);
        }

    cout << "DISPLAY : " << endl;
    tempUser_store = display.front().front().processTime;
    display.front().pop();
    readyQueue.push(display.front());
    display.pop();  
}

void INPUT(){
    int newInputTime = 0;
    pStatus.pstat = blocked;
    int input_Time = input.front().front().processTime;
    if(readyQueue.front().front().processName == "CORE" && (readyQueue.front().front().processTime < input_Time)){
        currentTime = currentTime + input_Time;
        if(tempUser_store > crt){
            currentTime = currentTime - tempUser_store;
        }
    }
    else if(tempUser_store > crt){
        currentTime = currentTime - tempUser_store + input_Time;
    }

    if(((input_Time + crt) > tempUser_store) && (tempUser_store!= 0)){
            currentTime = currentTime - ((input_Time + crt) - tempUser_store);
        }

    cout << "INPUT : " << endl;
    tempUser_store = input.front().front().processTime;
    input.front().pop();
    readyQueue.push(input.front());
    input.pop();    
}

void ROUND_ROBIN(){
    readyQueue.push(readyQueue.front());
    readyQueue.pop();
}

void CORE(int coreIdent){ //coreIdent, identifies which core is being used
    
    crt = readyQueue.front().front().processTime; //core request time
    
    ncore[coreIdent].cstat = occupied;
    pStatus.pstat = running;


    if(crt < slice){
        cout << "Core request time is less than slice" <<endl;
        if((temp_store < crt) || (tempUser_store < crt)){
            currentTime = currentTime + crt;
            temp_store = 0;
        }
        
        readyQueue.front().pop();
        ncore[coreIdent].cstat = empty;


    }
    
    else{
        crt = crt - slice;
        currentTime += slice;
        readyQueue.front().front().processTime = crt;
        cout << "core request time is greter than slice" << " " << currentTime << endl;
        ROUND_ROBIN();
        ncore[coreIdent].cstat = empty;                     //Process ID has been finished yet due to messed up input   

    }
    
}

void printOutput(){
    int counter = 0;
    cout << endl << endl;
    cout << "CURRENT STATE OF THE SYSTEM AT t = " << currentTime << "ns:" << endl;
    for(int i = 0; i < coreNum; i++){
        if(ncore[i].cstat == occupied){
            counter++;
        }
    }
    cout << "Current number of busy cores: " << counter << endl;
    
    cout << "READY QUEUE:" << endl;
    
    if(!readyQueue.empty()){
        cout << readyQueue.size() << endl;
    }
    else{
        cout << "empty" << endl;
    }

    cout << "DISK QUEUE:" << endl;
    if(!diskQueue.empty()){
        cout << readyQueue.size() << endl;
    }
    else{
        cout << "empty" << endl;
    }

    cout << "PROCESS TABLE:" << endl;
    int i = 0;
    while(i <= readyQueue.size()){
        cout << "Process " << readyQueue.front().front().processID << " started at " << ptime.processStartTime  << "ms and is " << "TERMINATED" << endl;
        i++;
    }
}

void Execution(){
    int i = 0;
    fileInput();

    while(!readyQueue.empty()){
        
        while(!readyQueue.front().empty()){
        
            if(readyQueue.front().front().processName == "NEW"){
                ptime.processStartTime = readyQueue.front().front().processTime;
                if(ptime.processStartTime > currentTime){
                    currentTime = ptime.processStartTime;
                
                }
                readyQueue.front().pop();
                pStatus.pstat = running;
                cout << "Current Time : " << currentTime <<  endl;

            }
            else{
        
                if(readyQueue.front().front().processName == "CORE"){
                    do{
                        if(ncore[i].cstat == empty){
                            CORE(i);
                            cout << currentTime << " seconds" << endl;
                            break;
                        }
                        else{ cout << "Core " << i << "is in use " << endl;
                        }

                        i++;
                    }while( i <= coreNum);

                }

                else if(readyQueue.front().front().processName == "DISK"){
                    diskQueue.push(readyQueue.front());
                    readyQueue.pop();
                    DISK();
                }   
                else if(readyQueue.front().front().processName == "DISPLAY"){
                    display.push(readyQueue.front());
                    readyQueue.pop();
                    DISPLAY();
                }   
                else if(readyQueue.front().front().processName == "INPUT"){
                    input.push(readyQueue.front());
                    readyQueue.pop();
                    INPUT();
                }   
            }
        }

        if(!readyQueue.empty()){
            printOutput();
            readyQueue.pop();
            
        }
    }
    
}

int main(){
    Execution();
    return 0;
} 