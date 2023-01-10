#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "dx2d.h"
#include "Bitmap.h"

Dx2d *dx2d;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, PWSTR cmdLine, int cmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        instance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, cmdShow);
    Dx2d dx2d_mem(hwnd);
    dx2d = &dx2d_mem;
    Bitmap bitmap(dx2d->render_target());

    // Run the message loop.

    MSG msg = { };
    auto last_tick = GetTickCount64();
    do {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message != WM_QUIT) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            last_tick = GetTickCount64();
        }
        else {
            FLOAT delta = GetTickCount64() - last_tick;
            delta /= 1000.0;
            last_tick = GetTickCount64();
        }
    } while (msg.message != WM_QUIT);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    //case WM_PAINT:

        //f.draw(*dx2d);
        //return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}