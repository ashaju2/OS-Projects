/*--------------Client Program------------
Input Direction:
	$ gcc client.cpp -lstdc++ -o client
	$ ./client 
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>

using namespace std;

int main(){
	int client_port;
	char hostname[256];
	char buffer[2048];
	struct sockaddr_in server_Addr;
	struct hostent *server;
	int sock_SysCall;
	int r_message, w_message;

	
//---------------------------------------------------------------------
	while (true) { //Establish Host Address
		 cout << "Enter a server host name: " << endl;
		 cin >> hostname;
		 if((server = gethostbyname(hostname)) != NULL){
			 break;
		 }
 	}

	cout << "Enter port number: " << endl;
	cin >> client_port; 

	memset(&server_Addr,0,sizeof(server_Addr));

	memcpy( (char *) &server_Addr.sin_addr, server->h_addr, server->h_length); // memcopy sets the address 

	server_Addr.sin_family = server->h_addrtype;
	server_Addr.sin_port = htons(client_port);

	if ((sock_SysCall = socket(server->h_addrtype, SOCK_STREAM, 0) ) < 0) //gets the socket
		 return(-1);

	if (connect(sock_SysCall, (struct sockaddr *) &server_Addr, sizeof(server_Addr)) < 0) { // Connects to the server
		perror("Error connecting"); 	
	 	return(-1);
	}

	
//----------------------------------------------------------------------------
	bool next_Input = true;
	if(next_Input != false){
		cout << "Enter a username: " << endl;
		cin >> buffer;
		string userName = buffer;
		if(string(buffer) == "Terminate"){
			w_message = write(sock_SysCall, buffer, sizeof(buffer));
			if (w_message < 0){ 
		         perror("ERROR writing to socket");
		    }
			next_Input = false;
		}
		else{
			w_message = write(sock_SysCall, buffer, sizeof(buffer));
			if (w_message < 0) 
		         perror("ERROR writing to socket");
		    
		    //cout << sizeof(buffer) << endl;

		    r_message = read(sock_SysCall, buffer, sizeof(buffer));
			if (r_message < 0) 
		         perror("ERROR writing to socket");
		    cout << "The public for user " << userName << " is " << buffer << endl;
		    //cout << strlen(buffer) << endl;
		}
	}

	close(sock_SysCall);
	 	
}

