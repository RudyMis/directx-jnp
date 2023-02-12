#include "Win32App.h"

#include "Win32App.h"
#include <windowsx.h>
#include <cmath>

Win32App::Win32App(UINT height, UINT width) :
	height(height),
	width(width),
	engine(height, width),
	hwnd(nullptr) {}

Win32App::~Win32App() {}

HRESULT Win32App::init_window(HINSTANCE instance) {
	HRESULT hr = S_OK;

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win32App::wnd_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = instance;
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wcex.lpszClassName = L"main";

	RegisterClassEx(&wcex);

	hwnd = CreateWindow(
		L"main",
		L"Snow Globe",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		nullptr,
		nullptr,
		instance,
		this);

	if (!hwnd) {
		return E_ABORT;
	}

	float dpi = GetDpiForWindow(hwnd);

	SetWindowPos(
		hwnd,
		nullptr,
		0,
		0,
		static_cast<int>(std::ceil(640.f * dpi / 96.f)),
		static_cast<int>(std::ceil(480.f * dpi / 96.f)),
		SWP_NOMOVE
	);
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);
	engine.on_init(hwnd);

	return S_OK;
}

void Win32App::run_message_loop() {
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Win32App::on_resize(UINT width, UINT height) {
}

LRESULT Win32App::wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	if (message == WM_CREATE) {
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lparam;
		Win32App* app = (Win32App*)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app)
		);
		return 1;
	}

	Win32App* app = reinterpret_cast<Win32App*>(static_cast<LONG_PTR>(
		::GetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA
		)));

	if (!app) {
		return DefWindowProc(hwnd, message, wparam, lparam);
	}

	switch (message) {
	case WM_PAINT:
	{
		if (!app->engine.is_init()) {
			return 0;
		}
		if (FAILED(app->engine.on_update())) {
			return 1;
		}
		if (FAILED(app->engine.on_render())) {
			return 1;
		}
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 1;
	}
	case WM_KEYDOWN:
		if (app->engine.is_init()) {
			app->engine.on_key_down(static_cast<UINT8>(wparam));
		}
		return 0;

	case WM_KEYUP:
		if (app->engine.is_init()) {
			app->engine.on_key_up(static_cast<UINT8>(wparam));
		}
		return 0;

	case WM_LBUTTONDOWN:
	{
		//auto pt = Point2(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		/*if (!std::holds_alternative<std::monostate>(app->clicked_object)) {
			return 0;
		}
		app->check_for_click(app->color_picker, pt);
		app->check_for_click(app->rules_canvas, pt);
		app->check_for_click(app->button, pt);*/
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (!(wparam & MK_LBUTTON)) {
			break;
		}
		//auto pt = Point2(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		/*std::visit(match{
			[](auto arg) {},
			[&]<clickable T>(std::reference_wrapper<T> obj) {
				obj.get().on_drag(pt, app->ctx);
			} }, app->clicked_object);*/
		return 0;
	}
	case WM_LBUTTONUP:
	{
		return 0;
	}
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}

 