#include <iostream>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib") 
using namespace std;



void main()
{
	//initialise winsoc
	WSADATA wsData;
	WORD ver = MAKEWORD(2,2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0) 
	{
		cerr << "cant initialise winsock! Quitting." <<endl;
		return;
	}
	//create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM,0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "cant create socket! Quitting." << endl;
		return;
	}

	//bind socket to an IP address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint,sizeof(hint));

	// Tell Winsoc the socket is for listening 
	listen(listening,SOMAXCONN);
	
	//Listen for a connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client,&clientsize);

	char host[NI_MAXHOST]; // CLIENTS REMOTE NAME
	char service[NI_MAXSERV]; // PORT CLIENT IS CONNECTED ON

	ZeroMemory(host, NI_MAXHOST); // this is the same as memset(host,0,NI_MAXHOST) but im not about multi platform today
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << "connected on port" << service << endl;    
	}
	else 
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << "host connected on port " << ntohs(client.sin_port) << endl;

	}
	//Close listening socket
	closesocket(listening);
	//While loop : accept and echo message back to client
	char buf[4096]; // really I should set this dynamically and read until there are no bytes but one step at a time god damn this is my 2nd c++ program will you lay off? 4k is more than enough relax.

	while (true)
	{
		ZeroMemory(buf, 4096);
		//wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096,0);
		if (bytesReceived == SOCKET_ERROR) 
		{
			cerr << "err in recv() quitting" << endl;
			break;
		}

		if (bytesReceived == 0) 
		{
			cerr << "client disconnected" << endl;
			break;
		}

		cout << string(buf, 0, bytesReceived) << endl;
		//echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);

	}
	//close socket
	closesocket(clientSocket);

	//Shutdown winsoc
	WSACleanup();
}