#include "window.hpp"
#include <iostream>
#include <thread>
#include <chrono>

// Global window registry
std::unordered_map<WindowHandle, WindowData*> g_windows;

#ifdef _WIN32
// ========== WINDOWS IMPLEMENTATION ==========

static bool g_classRegistered = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Encontra dados da janela
    auto it = g_windows.find(hwnd);
    if (it == g_windows.end()) {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    WindowData* data = it->second;
    
    switch (uMsg) {
        case WM_CLOSE:
            data->shouldClose = true;
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_SIZE: {
            data->width = LOWORD(lParam);
            data->height = HIWORD(lParam);
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Pinta fundo branco
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
            
            // Desenha texto de exemplo
            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);
            DrawTextA(hdc, "JP Window - Press ESC to close", -1, &rect, 
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RegisterWindowClass() {
    if (g_classRegistered) return;
    
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "JPWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    
    if (RegisterClassA(&wc)) {
        g_classRegistered = true;
    }
}

WindowHandle jp_create_window(const std::string& title, int width, int height) {
    RegisterWindowClass();
    
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    
    // Calcula tamanho da janela incluindo bordas
    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    
    // Cria janela
    HWND hwnd = CreateWindowExA(
        0,                          // dwExStyle
        "JPWindowClass",            // lpClassName
        title.c_str(),              // lpWindowName
        WS_OVERLAPPEDWINDOW,        // dwStyle
        CW_USEDEFAULT,              // X
        CW_USEDEFAULT,              // Y
        windowWidth,                // nWidth
        windowHeight,               // nHeight
        nullptr,                    // hWndParent
        nullptr,                    // hMenu
        hInstance,                  // hInstance
        nullptr                     // lpParam
    );
    
    if (!hwnd) {
        std::cerr << "Failed to create window. Error: " << GetLastError() << std::endl;
        return nullptr;
    }
    
    // Armazena dados da janela
    WindowData* data = new WindowData();
    data->handle = hwnd;
    data->shouldClose = false;
    data->title = title;
    data->width = width;
    data->height = height;
    
    g_windows[hwnd] = data;
    
    return hwnd;
}

void jp_show_window(WindowHandle window) {
    if (window) {
        ShowWindow(window, SW_SHOW);
        UpdateWindow(window);
    }
}

bool jp_poll_events(WindowHandle window) {
    if (!window) return false;
    
    auto it = g_windows.find(window);
    if (it == g_windows.end()) return false;
    
    WindowData* data = it->second;
    
    MSG msg;
    while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
        
        if (msg.message == WM_QUIT) {
            data->shouldClose = true;
        }
    }
    
    return !data->shouldClose;
}

void jp_destroy_window(WindowHandle window) {
    if (!window) return;
    
    auto it = g_windows.find(window);
    if (it != g_windows.end()) {
        delete it->second;
        g_windows.erase(it);
    }
    
    DestroyWindow(window);
}

void jp_set_window_title(WindowHandle window, const std::string& title) {
    if (window) {
        SetWindowTextA(window, title.c_str());
        
        auto it = g_windows.find(window);
        if (it != g_windows.end()) {
            it->second->title = title;
        }
    }
}

bool jp_is_key_pressed(int keyCode) {
    return (GetAsyncKeyState(keyCode) & 0x8000) != 0;
}

void jp_sleep(int milliseconds) {
    Sleep(milliseconds);
}

#elif __linux__
// ========== LINUX IMPLEMENTATION ==========

static Display* g_display = nullptr;
static int g_screen = 0;

WindowHandle jp_create_window(const std::string& title, int width, int height) {
    if (!g_display) {
        g_display = XOpenDisplay(nullptr);
        if (!g_display) {
            std::cerr << "Cannot open X display" << std::endl;
            return 0;
        }
        g_screen = DefaultScreen(g_display);
    }
    
    Window root = RootWindow(g_display, g_screen);
    
    // Cria janela
    Window window = XCreateSimpleWindow(
        g_display, root,
        0, 0, width, height, 1,
        BlackPixel(g_display, g_screen),
        WhitePixel(g_display, g_screen)
    );
    
    if (!window) {
        std::cerr << "Failed to create X11 window" << std::endl;
        return 0;
    }
    
    // Configura propriedades
    XStoreName(g_display, window, title.c_str());
    XSelectInput(g_display, window, 
                ExposureMask | KeyPressMask | KeyReleaseMask | 
                ButtonPressMask | StructureNotifyMask);
    
    // Configura WM_DELETE_WINDOW
    Atom wmDelete = XInternAtom(g_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(g_display, window, &wmDelete, 1);
    
    // Armazena dados da janela
    WindowData* data = new WindowData();
    data->handle = window;
    data->shouldClose = false;
    data->title = title;
    data->width = width;
    data->height = height;
    data->display = g_display;
    
    g_windows[window] = data;
    
    return window;
}

void jp_show_window(WindowHandle window) {
    if (window && g_display) {
        XMapWindow(g_display, window);
        XFlush(g_display);
    }
}

bool jp_poll_events(WindowHandle window) {
    if (!window || !g_display) return false;
    
    auto it = g_windows.find(window);
    if (it == g_windows.end()) return false;
    
    WindowData* data = it->second;
    
    XEvent event;
    while (XCheckWindowEvent(g_display, window, -1, &event)) {
        switch (event.type) {
            case DestroyNotify:
                data->shouldClose = true;
                break;
                
            case ClientMessage:
                data->shouldClose = true;
                break;
                
            case Expose:
                // Redesenha janela
                break;
        }
    }
    
    return !data->shouldClose;
}

void jp_destroy_window(WindowHandle window) {
    if (!window || !g_display) return;
    
    auto it = g_windows.find(window);
    if (it != g_windows.end()) {
        delete it->second;
        g_windows.erase(it);
    }
    
    XDestroyWindow(g_display, window);
    XFlush(g_display);
}

void jp_set_window_title(WindowHandle window, const std::string& title) {
    if (window && g_display) {
        XStoreName(g_display, window, title.c_str());
        XFlush(g_display);
        
        auto it = g_windows.find(window);
        if (it != g_windows.end()) {
            it->second->title = title;
        }
    }
}

bool jp_is_key_pressed(int keyCode) {
    // Implementação simples para Linux
    // Em implementação real, seria necessário rastrear estado das teclas
    return false;
}

void jp_sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

#elif __APPLE__
// ========== MACOS IMPLEMENTATION ==========

// Implementação macOS seria feita com Objective-C++
// Por simplicidade, deixamos as funções vazias

WindowHandle jp_create_window(const std::string& title, int width, int height) {
    std::cout << "macOS window creation not implemented yet" << std::endl;
    return nullptr;
}

void jp_show_window(WindowHandle window) {}
bool jp_poll_events(WindowHandle window) { return false; }
void jp_destroy_window(WindowHandle window) {}
void jp_set_window_title(WindowHandle window, const std::string& title) {}
bool jp_is_key_pressed(int keyCode) { return false; }
void jp_sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

#endif
