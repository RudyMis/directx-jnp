#pragma once

#pragma once

#include "helper.h"
#include "Engine.h"

// <3
//template<typename T>
//concept clickable = requires (T & t, const Point2 & p, Context & ctx) {
//	t.on_click(p, ctx);
//	t.on_drag(p, ctx);
//	t.on_release(ctx);
//
//	{ t.contains(p) } -> std::convertible_to<bool>;
//};

class Win32App {
public:
	Win32App(UINT height, UINT width);
	~Win32App();

	HRESULT init_window(HINSTANCE instance);
	void run_message_loop();

private:

	void on_resize(UINT width, UINT height);

	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

	HWND hwnd;

	Engine engine;

	UINT width;
	UINT height;
};