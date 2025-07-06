#pragma once

#ifdef _WIN32
    #include <windows.h>
    #include <windowsx.h>
    typedef HWND WindowHandle;
#elif __linux__
    #include <X11/Xlib.h>
    #include <X11/keysym.h>
    typedef Window WindowHandle;
#elif __APPLE__
    #include <Cocoa/Cocoa.h>
    typedef void* WindowHandle;  // NSWindow*
#endif

#include <string>
#include <unordered_map>

// Virtual Key Codes (compatível com Win32)
#define VK_SPACE    32
#define VK_ENTER    13
#define VK_ESCAPE   27
#define VK_LEFT     37
#define VK_UP       38
#define VK_RIGHT    39
#define VK_DOWN     40

// Estrutura para armazenar dados da janela
struct WindowData {
    WindowHandle handle;
    bool shouldClose;
    std::string title;
    int width, height;
    
#ifdef __linux__
    Display* display;
#endif
};

// Global registry para armazenar dados das janelas
extern std::unordered_map<WindowHandle, WindowData*> g_windows;

// Funções exportadas para JP
extern "C" {
    // Window management
    WindowHandle jp_create_window(const std::string& title, int width, int height);
    void jp_show_window(WindowHandle window);
    bool jp_poll_events(WindowHandle window);
    void jp_destroy_window(WindowHandle window);
    void jp_set_window_title(WindowHandle window, const std::string& title);
    
    // Input
    bool jp_is_key_pressed(int keyCode);
    
    // Utility
    void jp_sleep(int milliseconds);
}

// Platform-specific helper functions
#ifdef _WIN32
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void RegisterWindowClass();
int Win32ToVK(int win32Key);
#elif __linux__
int X11ToVK(KeySym xkey);
#endif
