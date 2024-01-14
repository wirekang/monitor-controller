#include "monitor-controller.h"

#include <Windows.h>

#include <atomic>
#include <thread>

#include "action.h"

void on();
void off();

MonitorController::MonitorController() : nextActionId(0), validActionId(0){};

void MonitorController::schedule(Action action, SEC delay) {
  int actionId = ++nextActionId;
  validActionId.store(actionId);
  std::thread t(&MonitorController::invoke, this, action, actionId, delay);
  t.detach();
}

void MonitorController::invoke(Action action, int actionId, SEC delay) {
  std::this_thread::sleep_for(std::chrono::duration<int>(delay));
  if (validActionId != actionId) {
    return;
  }

  switch (action) {
    case Action::on:
      return on();
    case Action::off:
      return off();
  }
}

void on() { mouse_event(MOUSEEVENTF_MOVE, 0, 2, 0, 0); }

void off() { SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2); }