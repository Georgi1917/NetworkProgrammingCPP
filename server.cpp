#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int __cdecl main(void) {

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	WSADATA wsaData;

	char recvbuf[DEFAULT_BUFLEN];
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

	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0) {

			std::cout << "Bytes Received: " << iResult << std::endl;

			iSendResult = send(ClientSocket, recvbuf, recvbuflen, 0);

			if (iSendResult == SOCKET_ERROR) {

				std::cout << "Send failed, error: " << WSAGetLastError() << std::endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;

			}

			std::cout << "Bytes send: " << iSendResult << std::endl;

		}
		else if (iResult == 0) {

			std::cout << "Connection closed\n";

		}
		else {

			std::cout << "Recv failed: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;

		}


	} while (iResult > 0);

	iResult = shutdown(ClientSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {

		std::cout << "Shutdown failed " << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;

	}

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}