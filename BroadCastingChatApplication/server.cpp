#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")


// First step is iinitialization
bool Initialize(){
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2) , &data) == 0;
}

void InteractWithCient(SOCKET clientSocket, vector <SOCKET> &clientSocket){
    cout << "client connected ..." << endl;
    char buffer[4096];
    while(1){
        int byteRecieved = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(byteRecieved <= 0){
            cout << "client disconnected ..."  << endl;
            break;
        }
        string message(buffer, byteRecieved);
        cout << "Message from client " << message << endl;

        for(auto client : clients){
            if(client != clientSocket){
                send(client, message.c_str(), message.length(), 0);
            }
        }
    }

    auto it = find(clients.begin(), clients.end(), clientSocket);
    if(it != clients.end()){
        clients.erase(it);
    }
    closesocket(clientSocket);
}
int main()
{
    if(!Initialize()){
        cout << "Server initialization failed ..." << endl;
        return 1;
    }
    
    // Creating the socket
    // we will let decide the protocol to be chosen by the service provider by mentioning 0;
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket == INVALID_SOCKET){
        cout << "Socket creation failed ..." << endl;
        return 1;
    }

    // setting the address structure
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);

    if(InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1){
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if(bind(listenSocket, reinterpret_cast<sockaddr*>(serveraddr), sizeof(serveraddr)) == SOCKET_ERROR){
        cout << "binding failed ..." << endl;
        return 1;
    }

    if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR){
        cout << "listen failed ..." << endl;
        return 1;
    }

    cout << "Server is listening on port " << port << endl;

    vector <SOCKET> clients;
    while(1){
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if(clientSocket == INVALID_SOCKET){
            cout << "invalid client socket " << endl;
        }

        clients.push_back(clientSocket);
        thread t1(InteractWithCient, clientSocket, std::ref(clients));
        t1.detach();
    }
    close(listenSocket);
    
    WSACleanup();    
    return 0;
}