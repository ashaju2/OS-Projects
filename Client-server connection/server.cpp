/*--------------Server Program------------
Input Direction:
	$ gcc server.cpp -lstdc++ -o server
	$ ./server 
*/

#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>

using namespace std;

queue<string> readIn;
map<string, string> myMap;

int main(int argc, char** argv){

	int server_Port;
	string input;
    string user_ID;
    string pub_Key;
    string fileName;
    //char myname[MAXHOSTNAME+1];
    

//	Reads argument file into program and pushes each line to queue    
//------------------------------------------------------------------------------------------------
	cout << "Enter a file name: ";
	cin >> fileName;
	cout << "Enter server port number: ";
	cin >> server_Port;
	ifstream myfile (fileName.c_str());

	if(myfile.is_open()){
	    while(getline(myfile, input)) {
	        readIn.push(input);
	    }
	}
	else{
		cout << "Unable to open file" << endl;
	}
//------------------------------------------------------------------------------------------------
	while (!readIn.empty())
        {
            istringstream iss(readIn.front());
            iss >> user_ID;
            iss >> pub_Key;

            myMap[user_ID] = pub_Key; //creats a mapping of use_ID to the public Key
            
            readIn.pop();
        }

//--------------------------------------------------------------------------------------------------    
    int sock_SysCall;
    int accept_SockSysCall;
    int message;
    char buffer[2048];

    socklen_t client_addrLen;
    struct sockaddr_in server_Addr;
    struct sockaddr_in client_Addr;

    sock_SysCall = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&server_Addr, 0, sizeof(struct sockaddr_in));
    server_Addr.sin_family = AF_INET;
    server_Addr.sin_addr.s_addr = INADDR_ANY;
    server_Addr.sin_port = htons(server_Port);
    if (sock_SysCall < 0){ /* create socket */
 		perror("Error Opening socket");
 		return(-1);
 	}
 	if (bind(sock_SysCall, (struct sockaddr *) &server_Addr ,sizeof(server_Addr)) < 0) {
		 close(sock_SysCall);
		 return(-1); /* bind address to socket */
	}

loop:  	listen(sock_SysCall, 3); /* max # of queued connects */
 	client_addrLen = sizeof(client_Addr);

 	accept_SockSysCall = accept(sock_SysCall, (struct sockaddr *) &client_Addr, &client_addrLen);

 	if(accept_SockSysCall < 0){
 		cout << "Accept Error" << endl;
 	}
 	else{
 		cout << "Connection Accepted" << endl;
 	}

string r_message;

 	if(r_message != "Terminate"){
	 	message = read(accept_SockSysCall, buffer, sizeof(buffer));
	 	if(message < 0){
	 		cout << "Error reading" << endl;
	 	}
	 	else{
	 		cout << "Message: " << buffer << endl;
	 		//cout << sizeof(buffer) << endl;
	 	}

	 	r_message = buffer;

	 	strncpy(buffer, myMap[r_message].c_str(), sizeof(buffer) - 1);
		buffer[sizeof(buffer) - 1] = 0;

	 	message = write(accept_SockSysCall, buffer, sizeof(buffer));
	 	if(message < 0){
	 		cout << "Error writing" << endl;
	 	}
	 	else{
	 		cout << "Message Sent" << endl;
	 	}
		 
	 }
  	close(accept_SockSysCall);
 	if(r_message != "Terminate"){
		goto loop;
	 }

	close(sock_SysCall);

 	

return 0;
}