#define UNICODE
#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#include <SDKDDKVer.h>
#include <Windows.h>

#include <iostream>
#include <thread>

#include "listener.h"
#include "monitor-controller.h"
#include "ui.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
  MonitorController monitorController = {};
  Ui ui = {hInstance};
  Listener listener = {};
  std::thread listenThread([&ui, &listener, &monitorController]() {
    listener.listen(
        12824, [&monitorController](Action action, int delay) { monitorController.schedule(action, delay); },
        [&ui](const wchar_t *message) { ui.alert(message); });
  });
  ui.start();
  return 1;
}