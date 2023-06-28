#pragma once

#include "TrackerNodeConfig.h"

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


class SocketConnectionWrapper {
public:
	bool init(TrackerNodeConfig config);
	void close();
	bool sendMessage(string message);
	string receiveMessage();
	string getLastError() { return lastError;; };

private:
	WSADATA wsaData;
	SOCKET clientSocket;
	sockaddr_in serverAddress;
	string lastError;

};

