#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

void receiveMessage(SOCKET ConnectSocket) {

	char buffer[DEFAULT_BUFLEN];

	while(1) {

		int result = recv(ConnectSocket, buffer, (int)strlen(buffer), 0);

		if (result > 0) {

			buffer[result] = *"\0";
			std::cout << buffer << std::endl;

		}
		else if (result == 0) {

			std::cout << "Connection Closed\n";
			break;

		}
		else {

			std::cout << "Recv failed. " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			break;

		}

	}

}

int __cdecl main(int argc, char **argv) {

	int recvbuflen = DEFAULT_BUFLEN;

	char input[100];
	char recvbuf[DEFAULT_BUFLEN];

	SOCKET ConnectSocket = INVALID_SOCKET;

	WSADATA wsaData;

	struct addrinfo	* result = NULL,
					* ptr = NULL,
					hints;

	int iResult, sentResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		std::cout << "WSAStartup failed. " << result;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		std::cout << "getaddrinfo failed. " << iResult << std::endl;
		return 1;
	}

	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Unable to connect to server!" << std::endl;
		WSACleanup();
		return 1;
	}

	if (iResult == SOCKET_ERROR) {
		std::cout << "Shutdown Failed. " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	std::thread recvThread(receiveMessage, ConnectSocket);

	while(1) {

		std::cin.getline(input, 100);
		sentResult = send(ConnectSocket, input, (int)strlen(input), 0);

		if (sentResult == 0) {

			std::cout << "Connection closed. " << std::endl;
			break;

		}
		else if (sentResult < 0) {

			std::cout << "Send failed. " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			break;

		}

	}

	closesocket(ConnectSocket);
	WSACleanup();
	recvThread.detach();

	return 0;

}