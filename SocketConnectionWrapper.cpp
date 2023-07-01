#include "SocketConnectionWrapper.h"

bool SocketConnectionWrapper::init(TrackerNodeConfig config)
{
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        lastError = "Failed to initialize winsock\n";
        return false;
    }

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        lastError = "Failed to create socket\n";
        WSACleanup();
        return false;
    }

    // Set up the server address and port
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(config.serverPort);  // Specify the port number

    // Convert the server IP address from string to binary form
    if (inet_pton(AF_INET, config.serverHost.c_str(), &(serverAddress.sin_addr)) != 1) {
        lastError = "Invalid server address\n";
        WSACleanup();
        return false;
    }

    cout << "Connecting..\n";
    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        lastError = "Failed to connect to the server\n";
        close();
        return false;
    }
    cout << "Connected!\n";
    return true;
}

void SocketConnectionWrapper::close() {
    closesocket(clientSocket);
    WSACleanup();
}

bool SocketConnectionWrapper::sendMessage(string message) {
    if (send(clientSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
        lastError = "Failed to send data to the server\n";
        close();
        return false;
    }
	return true;
}

string SocketConnectionWrapper::receiveMessage()
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    if (recv(clientSocket, buffer, sizeof(buffer) - 1, 0) == SOCKET_ERROR) {
        lastError = "Failed to receive data from the server\n";
        close();
        return string();
    }
	return string(buffer);
}
