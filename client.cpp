#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int __cdecl main(int argc, char **argv) {

	int recvbuflen = DEFAULT_BUFLEN;

	const char* sendBuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	SOCKET ConnectSocket = INVALID_SOCKET;

	WSADATA wsaData;

	struct addrinfo	* result = NULL,
					* ptr = NULL,
					hints;

	int iResult;

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

	iResult = send(ConnectSocket, sendBuf, (int)strlen(sendBuf), 0);

	if (iResult == SOCKET_ERROR) {
		std::cout << "Send failed. " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		std::cout << "Shutdown Failed. " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0) {
			std::cout << "Bytes Received: " << iResult << std::endl;
		}
		else if (iResult == 0) {
			std::cout << "Connection Closed" << std::endl;
		}
		else {
			std::cout << "Recv Failed. " << WSAGetLastError() << std::endl;
		}

	} while (iResult > 0);

	closesocket(ConnectSocket);
	WSACleanup();

	return 0;

}