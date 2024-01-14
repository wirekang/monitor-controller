#ifndef MONITOR_CONTROLLER_H_
#define MONITOR_CONTROLLER_H_
#include <atomic>

#include "action.h"

typedef int SEC;

class MonitorController {
 public:
  MonitorController();
  void schedule(Action action, SEC delay);

 private:
  std::atomic<int> nextActionId;
  std::atomic<int> validActionId;
  void invoke(Action action, int actionId, SEC delay);
};

#endif  // MONITOR_CONTROLLER_H_