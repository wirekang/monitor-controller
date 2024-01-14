#include "ui.h"

#include <Windows.h>
#include <shellapi.h>

#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include "resources.h"

constexpr auto ICON_ID = 1;
constexpr auto ICON_MSG = WM_USER + 0x1;

[[noreturn]] void panic(const WCHAR*);
HWND initWindow(HINSTANCE);
void initIcon(HINSTANCE, HWND);
void removeIcon(HWND);
LRESULT CALLBACK windowProc(HWND, UINT, WPARAM, LPARAM);

Ui::Ui(HINSTANCE hInstance) {
  hWnd = initWindow(hInstance);
  initIcon(hInstance, hWnd);
}

void Ui::start() {
  auto e = 0;
  MSG msg;
  int result = 0;
  while ((result = GetMessageW(&msg, hWnd, 0, 0)) != 0) {
    if (result == -1) {
      panic(L"GetMessage failed");
    }
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
}

void Ui::alert(const WCHAR* message) {
  std::thread t([message]() { MessageBoxW(NULL, message, L"monitor-controller", MB_APPLMODAL); });
  t.detach();
}

void panic(const WCHAR* message) {
  std::wstring buf;
  auto lastError = GetLastError();
  buf.append(message);
  buf.append(L": ");
  buf.append(std::to_wstring(lastError));
  MessageBoxW(NULL, buf.c_str(), L"[ERROR] monitor-controller", MB_OK);
  exit(1);
}

HWND initWindow(HINSTANCE hInstance) {
  wchar_t className[] = L"MainClass";

  WNDCLASSEXW clazz = {};
  clazz.cbSize = sizeof(WNDCLASSEXW);
  clazz.hInstance = hInstance;
  clazz.lpfnWndProc = windowProc;
  clazz.lpszClassName = className;
  if (RegisterClassExW(&clazz) == 0) {
    panic(L"RegisterClassEx failed");
  }

  auto hwnd = CreateWindowExW(0, className, NULL, 0, 0, 0, 10, 10, HWND_MESSAGE, NULL, hInstance, NULL);
  if (hwnd == NULL) {
    panic(L"CreateWindow failed");
  }
  return hwnd;
}

void initIcon(HINSTANCE hInstance, HWND hWnd) {
  auto hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON));
  NOTIFYICONDATAW icon = {};
  std::wcscpy(icon.szTip, L"monitor-controller");

  icon.cbSize = sizeof(NOTIFYICONDATAW);
  icon.hWnd = hWnd;
  icon.uID = ICON_ID;
  icon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
  icon.uCallbackMessage = ICON_MSG;
  icon.hIcon = hIcon;
  icon.uVersion = NOTIFYICON_VERSION_4;
  if (!Shell_NotifyIconW(NIM_ADD, &icon)) {
    panic(L"Icon add failed");
  }
  if (!Shell_NotifyIconW(NIM_SETVERSION, &icon)) {
    panic(L"Icon setversion failed");
  }
}

void removeIcon(HWND hWnd) {
  NOTIFYICONDATAW icon = {};
  icon.cbSize = sizeof(NOTIFYICONDATAW);
  icon.hWnd = hWnd;
  icon.uID = ICON_ID;
  Shell_NotifyIconW(NIM_DELETE, &icon);
}

LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  constexpr int IDM_QUIT = 100;
  switch (msg) {
    case WM_NCDESTROY:
      removeIcon(hWnd);
      break;
    case ICON_MSG:
      switch (LOWORD(lParam)) {
        case NIN_SELECT:
        case NIN_KEYSELECT:
        case WM_CONTEXTMENU:
          POINT pt;
          GetCursorPos(&pt);
          auto hMenu = CreatePopupMenu();
          InsertMenuW(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_QUIT, L"Quit monitor-controller");
          SetForegroundWindow(hWnd);
          TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);
          PostMessageW(hWnd, WM_NULL, 0, 0);
          break;
      }
      break;
    case WM_COMMAND:
      if (lParam == 0 && LOWORD(wParam) == IDM_QUIT) {
        exit(0);
      }
  }
  return DefWindowProcW(hWnd, msg, wParam, lParam);
}
