#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <map>
#include "hardware.h"
#include <server\handler\serverCmdHandler.cpp>
#include <windows.h>
#include <server\result\ServerResult.h>

#pragma comment(lib, "Ws2_32.lib")

const int BUFFER_SIZE = 8192; // 8kb

void startServer(const char* port);
void handleClient(SOCKET clientSocket);
bool ExecuteCmd(std::string& cmd, SOCKET clientSocket, char  buffer[1024], int bytesReceived);
bool handleExitCmd(std::string& cmd);
std::string flushReceivedBytes(char  currentCmd[1024], int& currentCmdCursor, int bytesReceived, char  buffer[1024]);
std::map<std::string, std::string> commands;

void SERVER_Init() {
    // Start the server in a separate thread with a restart mechanism
    std::thread serverThread([]() {
        while(true) {
            try {
                std::cout << "Starting server...\n";
                startServer("8182");
            }
            catch(const std::exception& e) {
                std::cerr << "Server crashed with exception: " << e.what() << "\n";
            }
            catch(...) {
                std::cerr << "Server crashed with an unknown error.\n";
            }

            // Wait a bit before restarting to avoid rapid restarts
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cerr << "Restarting server...\n";
        }
    });

    // Detach the thread so it runs independently
    serverThread.detach();

    std::cout << "Server is running in the background with auto-restart enabled.\n";
}

void startServer(const char* port) {
    WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET;
    struct addrinfo* result = nullptr, hints;

    // Initialize Winsock
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }

    // Set up the hints structure
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = IPPROTO_TCP; // TCP
    hints.ai_flags = AI_PASSIVE;     // For binding

    // Resolve the server address and port
    if(getaddrinfo(nullptr, port, &hints, &result) != 0) {
        std::cerr << "getaddrinfo failed.\n";
        WSACleanup();
        return;
    }

    // Create a socket
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(listenSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Bind the socket
    if(bind(listenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    // Listen on the socket
    if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    std::cout << "Waiting for client connection on port " << port << "...\n";

    // Accept a client socket
    while(true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if(clientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed: " << WSAGetLastError() << "\n";
            closesocket(listenSocket);
            WSACleanup();
            continue;
        }

        std::cout << "Client connected.\n";

        handleClient(clientSocket);
    }

    // Cleanup
    closesocket(listenSocket);
    WSACleanup();
}

void handleClient(SOCKET clientSocket) {
    char receiveBuffer[BUFFER_SIZE];
    ServerResult serverResult;
    char sendBuffer[BUFFER_SIZE];
    char currentCmd[BUFFER_SIZE];
    int currentCmdCursor = 0;
    int bytesReceived;

    std::cout << "Client handler started.\n";

    CmdCallback replyCallBack = [clientSocket](ServerResult* result) {
        //OutputDebugString(("Returning command result : " + result + "\n").c_str());

        if(result->length > 0) {
            // 4 bytes for length (32 bits value)
            char sendBuffer[BUFFER_SIZE];
            uint32_t networkBigEndian = htonl(result->length);
            std::memcpy(sendBuffer, &networkBigEndian, 4);

            if(send(clientSocket, sendBuffer, 4, 0) == SOCKET_ERROR) {
                std::cerr << "Error sending data to client.\n";
                return;
            }

            int sendSize = min(BUFFER_SIZE, result->length);
            std::memcpy(sendBuffer, result->result, sendSize);

            for(int i = sendSize; i < result->length; i += BUFFER_SIZE) {
                sendSize = min(result->length - i, BUFFER_SIZE);
                if(send(clientSocket, sendBuffer, sendSize, 0) == SOCKET_ERROR) {
                    std::cerr << "Error sending data to client.\n";
                    break;
                }
            }
        }
    };

    // Client-server communication loop
    while(true) {
        // Receive data from the client
        bytesReceived = recv(clientSocket, receiveBuffer, sizeof(receiveBuffer) - 1, 0);
        if(bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "Client disconnected or error occurred.\n";
            break;
        }

        receiveBuffer[bytesReceived] = '\0';
        std::string cmd = flushReceivedBytes(currentCmd, currentCmdCursor, bytesReceived, receiveBuffer);
        if(handleExitCmd(cmd)) {
            break;
        }

        serverResult.length = 0;
        executeServerCmd(cmd, replyCallBack, &serverResult);
    }

    closesocket(clientSocket);
    std::cout << "Client handler terminated.\n";
}

bool handleExitCmd(std::string& cmd)
{
    if(cmd == "exit") {
        return true;
    }
    return false;
}

std::string flushReceivedBytes(char currentCmd[], int& currentCmdCursor, int bytesReceived, char buffer[]) {
    // transfer received data to currentCmd
    // then the character ";" is received, the currentCmd is executed
    // then the rest of the buffer is appended to currentCmd for the next command
    currentCmd[currentCmdCursor] = '\0';
    for(int i = 0; i < bytesReceived; ++i) {
        if(buffer[i] == '\r') {
            continue; // Ignore cariage return (Windows)
        }
        else if(buffer[i] == '\n') {
            currentCmd[currentCmdCursor] = '\0';
            currentCmdCursor = 0; // Reset for the next command
            return currentCmd;
        }
        else {
            if(currentCmdCursor < BUFFER_SIZE - 1) {
                currentCmd[currentCmdCursor++] = buffer[i];
            }
            else {
                currentCmd[currentCmdCursor] = '\0';
                return currentCmd;
            }
        }
    }
    return "";
}
