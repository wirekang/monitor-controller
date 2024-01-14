#ifndef LISTENER_H_
#define LISTENER_H_

#include <functional>

#include "action.h"

class Listener {
 public:
  Listener() = default;
  void listen(unsigned short port, std::function<void(Action, int)> onAction,
              std::function<void(const wchar_t *message)> onError);
};

#endif  // LISTENER_H_