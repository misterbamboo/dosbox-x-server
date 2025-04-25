#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "hardware.h"

#pragma comment(lib, "Ws2_32.lib")

/*#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "bitmapinfoheader.h"
#include "dosbox.h"
// #include "control.h"
#include "logging.h"
#include "setup.h"
#include "support.h"
#include "mem.h"
// #include "mapper.h"
#include "pic.h"
#include "vga.h"
#include "mixer.h"
// #include "render.h"
// #include "cross.h"
#include "wave_mmreg.h"

#if (C_SSHOT) || (C_AVCODEC)
#include <zlib.h>
#include <png.h>
#include "../libs/zmbv/zmbv.h"
#endif

#include "riff_wav_writer.h"
#include "avi_writer.h"
#include "rawint.h"

#include <map>
*/

void StartServer(const char* port);
void HandleClient(SOCKET clientSocket);
bool ExecuteCmd(std::string& cmd, SOCKET clientSocket, char  buffer[1024], int bytesReceived);
bool HandleExitCmd(std::string& cmd);
std::string FlushReceivedBytes(char  currentCmd[1024], int& currentCmdCursor, int bytesReceived, char  buffer[1024]);

void SERVER_Init() {
    // Start the server in a separate thread with a restart mechanism
    std::thread serverThread([]() {
        while(true) {
            try {
                std::cout << "Starting server...\n";
                StartServer("8182");
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

void StartServer(const char* port) {
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

        HandleClient(clientSocket);
    }

    // Cleanup
    closesocket(listenSocket);
    WSACleanup();
}

void HandleClient(SOCKET clientSocket) {
    char buffer[1024];
    char currentCmd[1024];
    int currentCmdCursor = 0;
    int bytesReceived;

    std::cout << "Client handler started.\n";

    // Client-server communication loop
    while(true) {
        // Receive data from the client
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if(bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "Client disconnected or error occurred.\n";
            break;
        }

        // Null-terminate the received data
        buffer[bytesReceived] = '\0';
        std::cout << "Received from client: " << buffer << "\n";

        std::string cmd = FlushReceivedBytes(currentCmd, currentCmdCursor, bytesReceived, buffer);

        bool exit = ExecuteCmd(cmd, clientSocket, buffer, bytesReceived);
        if(exit) break;
    }

    // Cleanup
    closesocket(clientSocket);
    std::cout << "Client handler terminated.\n";
}

bool ExecuteCmd(std::string& cmd, SOCKET clientSocket, char  buffer[1024], int bytesReceived)
{
    std::string result = "";
    if(!cmd.empty()) {
        std::cout << "Executing command: " << cmd << "\n";
        if(HandleExitCmd(cmd))
        {
            return true; // exit the connexion
        }
        else if(cmd == "capture")
        {
            InitServerScreenCapture();
            while(ServerCaptureInProcess)
            {
                // wait for the capture to finish
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            result = "Capture command executed.\n";
        }
        else
        {
            std::cout << "Unknown command: " << cmd << "\n";
        }

        // Echo the data back to the client
        if(send(clientSocket, result.c_str(), result.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Error sending data to client.\n";
            return true; // exit the connexion
        }
    }

    return false;
}

bool HandleExitCmd(std::string& cmd)
{
    if(cmd == "exit") {
        std::cout << "Client requested to exit.\n";
        return true;
    }
    return false;
}

std::string FlushReceivedBytes(char currentCmd[1024], int& currentCmdCursor, int bytesReceived, char  buffer[1024])
{
    // transfer received data to currentCmd
    // then the character ";" is received, the currentCmd is executed
    // then the rest of the buffer is appended to currentCmd for the next command
    currentCmd[currentCmdCursor] = '\0';
    for(int i = 0; i < bytesReceived; ++i) {
        if(buffer[i] == '\n') {
            currentCmd[currentCmdCursor] = '\0';
            return currentCmd;
            // Execute the command here (e.g., send it to the emulator)
            currentCmdCursor = 0; // Reset for the next command
        }
        else {
            if(currentCmdCursor < sizeof(currentCmd) - 1) {
                currentCmd[currentCmdCursor++] = buffer[i];
            }
        }
    }
    return "";
}
