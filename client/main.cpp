#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

/*
Client code Steps:
1. Initialize WinSock Library
2. create the Socket
3. Get the server IP(localhost) & port used (12345)
4. Connect to server
5. Send/ Recv
6. Close the socket
7. Cleanup the WinSock

*/

bool Initialize() {		// Boiler Plate code
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}


void SendMsg(SOCKET listenSocket) {
	cout << "Enter your chat name: " << endl;
	string name;
	getline(cin, name);
	string userMsg;

	while (1) { 
		getline(cin, userMsg);
		if (userMsg == "quit") {
			cout << "Stopping your chat session..." << endl;
			break;
		}
		string msg = name + ": " + userMsg;

		int bytesent = send(listenSocket, msg.c_str(), msg.length(), 0);

		if (bytesent == SOCKET_ERROR) {
			cout << "Message sending failed" << endl;
			break;
		}
	}

	closesocket(listenSocket);
	WSACleanup();
}

void ReceiveMsg(SOCKET listenSocket) {
	char buffer[4096];
	int bytesrecvd;
	string msg;
	while (1) {
		bytesrecvd = recv(listenSocket, buffer, sizeof(buffer), 0);
		if (bytesrecvd <= 0) {			// 0 : clinet raised close call, <0 : listenSocket
			cout << "Disconnected from server!!" << endl;
			break;
		}
		else {
			msg = string(buffer, bytesrecvd);
			cout << msg << endl;
		}

	}

	closesocket(listenSocket);
	WSACleanup();
}

int main() {
	cout << "Client program..." << endl;

	// 1. Initialize WinSock Library
	if (!Initialize()) {
		cout << "Winsock initiazlization failed!!" << endl;
		return 1;
	}

	// 2. create the Socket
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		cout << "Socket creation failed!!" << endl;
		return 1;
	}

	// 3. Get the server IP(localhost) & port used (12345)

	// create address structure
	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET; 
	serveraddr.sin_port = htons(port);

	// convert the ip address(0.0.0.0  -> localhost) to binary format and put it inside the sin_address
	if (inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr)) != 1) {
		cout << "Address struct setting failed!!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// 4. Connect to server
	if(connect(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Not able to connect to Server!!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Successfulyy conected to server on port: " << port << endl;

	thread senderThread(SendMsg, listenSocket);
	thread receiverThread(ReceiveMsg, listenSocket);

	senderThread.join();
	receiverThread.join();

	return 0;
}