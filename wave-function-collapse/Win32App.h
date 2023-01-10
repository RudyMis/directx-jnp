#pragma once

#include "helper.h"
#include "color_picker/color_picker.h"
#include "rules_canvas.h"
#include "generator.h"
#include "visualizer.h"
#include "bitmaps.h"
#include "button.h"
#include <variant>
#include <concepts>

// <3
template<typename T>
concept clickable = requires (T& t, const Point2& p, Context& ctx) {
	t.on_click(p, ctx);
	t.on_drag(p, ctx);
	t.on_release(ctx);

	{ t.contains(p) } -> std::convertible_to<bool>;
};

class Win32App {
public:
	Win32App();
	~Win32App();

	HRESULT init_window(HINSTANCE instance);
	void run_message_loop();

private:

	HRESULT create_device_independent_resources();
	HRESULT create_device_resources();

	void discard_device_resources();

	HRESULT on_render();

	void on_resize(UINT width, UINT height);
	
	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	
	template<clickable C>
	inline void check_for_click(C& obj, const Point2& p) {
		if (obj.contains(p)) {
			obj.on_click(p, ctx);
			clicked_object = std::ref(obj);
			InvalidateRect(hwnd, nullptr, FALSE);
		}
	}

	HWND hwnd;
	IWICImagingFactory* wic_factory;
	CComPtr<ID2D1Factory> dx_factory;
	CComPtr<ID2D1HwndRenderTarget> render_target;
	CComPtr<ID2D1SolidColorBrush> light_slate_gray_brush;

	CComPtr<IDWriteFactory> write_factory;
	CComPtr<IDWriteTextFormat> text_format;

	std::variant<std::monostate,
		std::reference_wrapper<ColorPicker>,
		std::reference_wrapper<RulesCanvas>,
		std::reference_wrapper<Button>> clicked_object;

	ColorPicker color_picker;
	RulesCanvas rules_canvas;
	Visualizer visualizer;
	Button button;

	Context ctx;
};

