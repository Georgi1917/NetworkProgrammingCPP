#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

void receiveMessage(SOCKET ClientSocket) {

	char buffer[DEFAULT_BUFLEN];

	while(1) {

		int result = recv(ClientSocket, buffer, DEFAULT_BUFLEN, 0);

		if (result > 0) {

			buffer[result] = *"\0";

			std::cout << buffer << std::endl;

		}
		else if (result == 0) {

			std::cout << "Connection closed\n" << std::endl;
			break;

		}
		else {

			std::cout << "Recv failed. " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			break;

		}

	}

}

int __cdecl main(void) {

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	WSADATA wsaData;

	char recvbuf[DEFAULT_BUFLEN];
	char input[100];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		std::cout << "WSAStartup failed. " << iResult << std::endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return 1;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Socket failed. " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		std::cout << "Bind Failed with Error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) {
		std::cout << "Listen() failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "accept failed: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ListenSocket);

	std::thread recvThread(receiveMessage, ClientSocket);

	while(1) {

		std::cin.getline(input, 100);
		iSendResult = send(ClientSocket, input, (int)strlen(input), 0);

		if (iSendResult == 0) {

			std::cout << "Connection closed" << std::endl;
			break;

		}
		else if (iSendResult < 0) {

			std::cout << "Send failed. " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			break;

		}

	}

	iResult = shutdown(ClientSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {

		std::cout << "Shutdown failed " << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;

	}

	closesocket(ClientSocket);
	WSACleanup();
	recvThread.detach();

	return 0;
}