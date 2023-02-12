#include "Win32App.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, PWSTR cmdLine, int cmdShow) {
    HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

    if (!SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        return -1;
    }
    Win32App app(800, 600);

    if (!SUCCEEDED(app.init_window(instance))) {
        CoUninitialize();
        return -1;
    }

    app.run_message_loop();
    CoUninitialize();

    return 0;
}