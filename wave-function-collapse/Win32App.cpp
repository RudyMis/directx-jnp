#include "Win32App.h"
#include <windowsx.h>
#include <cmath>

Win32App::Win32App() :
	hwnd(nullptr),
	dx_factory(nullptr),
	render_target(nullptr),
	light_slate_gray_brush(nullptr),
	ctx{},
	button{ {0.0f, 190.0f}, {200.0f, 50.0f} }
{
}

Win32App::~Win32App() {
	Bitmaps::wic_bitmaps.clear();
}

HRESULT Win32App::init_window(HINSTANCE instance) {
	HRESULT hr = S_OK;

	hr = create_device_independent_resources(); CHECK;

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win32App::wnd_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = instance;
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wcex.lpszClassName = L"wfc";
	wcex.hIcon = LoadIcon(instance, L"doggo.png"); // Nie dzia³a :c

	RegisterClassEx(&wcex);

	hwnd = CreateWindow(
		L"wfc",
		L"Artysta Malarz",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
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

	return hr;
}

void Win32App::run_message_loop() {
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT Win32App::create_device_independent_resources() {
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &dx_factory); CHECK;

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&write_factory)
	); CHECK;

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wic_factory)
	); CHECK;
	hr = Bitmaps::load_wic_bitmap_from_file(wic_factory, L"slider.bmp"); CHECK;

	hr = rules_canvas.create_path(dx_factory); CHECK;
	hr = button.create_text_format(write_factory);
	button.set_callback([&]() {
		visualizer.setup_animation(rules_canvas.get_pixels()); });

	return hr;
}

HRESULT Win32App::create_device_resources() {
	HRESULT hr = S_OK;
	if (render_target) {
		return hr;
	}

	RECT rc;
	GetClientRect(hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(
		rc.right - rc.left,
		rc.bottom - rc.top
	);

	hr = dx_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, size),
		&render_target
	); CHECK;

	Point2 rules_canvas_pos(20.0f, 260.0f);
	Point2 rules_canvas_size(160.0f, 160.0f);
	rules_canvas.init(rules_canvas_pos, {
		rules_canvas_pos.x + rules_canvas_size.x,
		rules_canvas_pos.y + rules_canvas_size.y });
	visualizer.init({ 200.0f, 0.0f }, { (FLOAT)size.width, (FLOAT)size.height });

	color_picker.create_resources(render_target);
	rules_canvas.create_resources(render_target);
	visualizer.create_resources(render_target);
	button.create_resources(render_target);

	return hr;
}

void Win32App::discard_device_resources() {
	color_picker.discard_resources();
	rules_canvas.discard_resources();
	visualizer.discard_resources();
	button.discard_resources();
	render_target.Release();
}

HRESULT Win32App::on_render() {
	HRESULT hr = S_OK;

	hr = create_device_resources(); CHECK;

	render_target->BeginDraw();
	render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	render_target->Clear(D2D1::ColorF(D2D1::ColorF::White));
	
	color_picker.draw(render_target);
	rules_canvas.draw(render_target);
	visualizer.draw(render_target);
	button.draw(render_target);

	hr = render_target->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET) {
		hr = S_OK;
		discard_device_resources();
	}
	return hr;
}

void Win32App::on_resize(UINT width, UINT height) {
	if (!render_target) {
		return;
	}

	render_target->Resize(D2D1::SizeU(width, height));
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
	case WM_SIZE:
	{
		UINT width = LOWORD(lparam);
		UINT height = HIWORD(lparam);
		app->on_resize(width, height);
		return 0;
	}
	case WM_DISPLAYCHANGE:
	{
		InvalidateRect(hwnd, nullptr, FALSE);
		return 0;
	}
	case WM_PAINT:
	{
		app->on_render();
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 1;
	}
	case WM_LBUTTONDOWN:
	{
		auto pt = Point2(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		if (!std::holds_alternative<std::monostate>(app->clicked_object)) {
			return 0;
		}
		app->check_for_click(app->color_picker, pt);
		app->check_for_click(app->rules_canvas, pt);
		app->check_for_click(app->button, pt);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (!(wparam & MK_LBUTTON)) {
			break;
		}
		auto pt = Point2(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		std::visit(match{
			[](auto arg) {},
			[&]<clickable T>(std::reference_wrapper<T> obj) {
				obj.get().on_drag(pt, app->ctx);
			} }, app->clicked_object);	
		return 0;
	}
	case WM_LBUTTONUP:
	{
		app->clicked_object = std::monostate{};
		return 0;
	}
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}
