#include "listener.h"

#include <WinSock2.h>
#include <Windows.h>

#include <functional>
#include <map>
#include <string>

#include "action.h"

constexpr auto BUF_SIZE = 512;

const std::map<std::string, Action> actions = {
    {"on", Action::on},
    {"off", Action::off},
};

void Listener::listen(unsigned short port, std::function<void(Action, int)> onAction,
                      std::function<void(const WCHAR* message)> onPanic) {
  WSADATA wsaData;
  if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
    WSACleanup();
    onPanic(L"WSAStartup failed");
  }

  SOCKADDR_IN serverAddr = {};
  SOCKADDR_IN clientAddr = {};
  char buffer[BUF_SIZE] = {0};
  int addrSize = sizeof(SOCKADDR_IN);

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (serverSocket == INVALID_SOCKET) {
    WSACleanup();
    onPanic(L"INVALID_SOCKET");
  }

  if (bind(serverSocket, (struct sockaddr*)&serverAddr, addrSize) == SOCKET_ERROR) {
    closesocket(serverSocket);
    WSACleanup();
    onPanic(L"Bind failed");
    exit(1);
  }

  while (true) {
    auto recvSize = recvfrom(serverSocket, (char*)buffer, BUF_SIZE, 0, (struct sockaddr*)&clientAddr, &addrSize);
    if (recvSize < 0) {
      onPanic(L"recvfrom returns error");
    }
    std::string message = {buffer};
    memset(buffer, 0, BUF_SIZE);
    auto i = message.find(',');
    auto action = actions.find(message.substr(0, i))->second;
    auto delay = stoi(message.substr(i + 1));
    onAction(action, delay);
  }
}
