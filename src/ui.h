#ifndef UI_H_
#define UI_H_

#include "Windows.h"

class Ui {
 public:
  Ui(HINSTANCE);
  [[noreturn]] void alert(const WCHAR*);
  void start();

 private:
  HWND hWnd;
};

#endif  // UI_H_