#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>

using namespace std;

#pragma comment(lib, "ws2_32.lib")		// for using socket lib, linking with ws2 lib

/*
Server code Steps:
1. Initialize WinSock Library
2. create the Socket
3. Get the IP(localhost) & port(ex: 12345) on which server should be running (other client will connect on this port)
4. Bind the Ip/port with the socket created.
5. Listening on the socket
6. Accept the connection from client (Currently dealing only blocking calls) (stopped till we don't get client connection)
7. Receive & send
8. Close the socket
9. Cleanup the WinSock

*/
	
bool Initialize() {		// Boiler Plate code
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

int main() {
	cout << "Server program..." << endl;

	// 1. Initialize WinSock Library
	if (!Initialize()) {
		cout << "Winsock initiazlization failed!!" << endl;
		return 1;
	}

	// 2. create the Socket
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0); // adsress family, stream(tcp/udp), protocol
	if (listenSocket == INVALID_SOCKET) {
		cout << "Socket creation failed!!" << endl;
		return 1;
	}

	// 3. Get the IP(localhost) & port(ex: 12345)

	// create address structure
	int port = 12345;
	string serveraddress = "0.0.0.0";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	// convert the ip address(0.0.0.0  -> localhost) to binary format and put it inside the sin_address
	if(inet_pton(AF_INET, serveraddress.c_str(), &serveraddr.sin_addr) != 1) {
		cout << "Address struct setting failed!!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// 4. Bind the Ip/port with the socket created.
	if(bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR){
		cout << "Socket binding failed!!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// 5. Listening on the socket
	if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR){ // 2ndParam: amt of client it can have in queue
		cout << "Socket listening failed!!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server has started listening on port: " << port << endl;

	// 6. Start Accepting the connection from client
	SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET) {
		cout << "Client Socket invalid!!" << endl;
		return 1;
	}

	// 7. Receive & send
	char buffer[4096];
	int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
	
	string message(buffer, bytesrecvd);
	cout << "Message from client: " << message << endl;

	// 8. Close the socket
	//closesocket(clientSocket);
	closesocket(listenSocket);
	
	// 9. Cleanup the WinSock
	WSACleanup();
	return 0;
}