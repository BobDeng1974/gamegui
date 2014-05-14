#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#include	<shlobj.h>

#include "uitest.h"
#include <guiplatform/renderer_ogl.h>

//#include <boost/filesystem.hpp>
#include <iostream>
#include <functional>

using namespace gui;

class gui_log : public log
{
public: 
	gui_log() : 
	  m_hFile(INVALID_HANDLE_VALUE)
	{
		  wchar_t	wpath[MAX_PATH];
		  // init app data path
		  SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, wpath);

		  char buff[256];
		  int size = GetModuleFileNameA(NULL, buff, 256);

		  std::string path(buff, size);

		  int pos = path.find_last_of('\\');

		  path = path.substr(0, pos);


		  std::string log = path + "/guitest.log";
		  //std::wstring log(wpath);
		  //log += L"\\RGDEngine\\guitest.log";
		  m_hFile = CreateFileA(log.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	}

	~gui_log() {
		if(m_hFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hFile);
	}

	void write(log::level l, const std::string& msg) 
	{
		static const char* type2str[log::log_levels_num] = {"sys", "msg", "wrn", "ERR", "CRITICAL"};
		const char* type = type2str[l];

		SYSTEMTIME st;
		GetLocalTime(&st);

		static char con_timestamp[32] = {0};
		_snprintf(con_timestamp, 32, "[%02d:%02d:%02d][%s] ", st.wHour, st.wMinute, st.wSecond, type);
		std::cout << con_timestamp << msg.c_str() << std::endl;

		if(m_hFile == INVALID_HANDLE_VALUE)
			return;

		static char timestamp[32] = {0};
		_snprintf(timestamp, 32, "[%04d.%02d.%02d %02d:%02d:%02d][%s] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, type);
	
		std::string m = timestamp + msg + "\n";
		DWORD len = (DWORD)m.length();
		WriteFile(m_hFile, m.c_str(), len, &len, 0);
	}

	HANDLE m_hFile;
};

gui_log g_log;

#include "../common/gui_filesystem.h"

ui_test_application::ui_test_application(int w, int h, const char* title)
	: BaseApplication(w, h, title)
	, m_render(NULL)
	, m_system(NULL)
	//, window(math::vec2i(x, y), math::vec2i(w, h), title, 0, WS_BORDER | WS_CAPTION | WS_SYSMENU)
	//, m_render_device(m_filesystem)
	, m_elapsed(0)
	, m_active(true)
{
	using namespace std::placeholders;
	env.render_cb = std::bind(&ui_test_application::render, this);
	env.update_cb = std::bind(&ui_test_application::update, this);

	//show();
	update();	
}

ui_test_application::~ui_test_application()
{
	m_system.reset();
	m_render.reset();
}

void ui_test_application::run()
{	
	createGUISystem();
	BaseApplication::run();
}

void ui_test_application::createGUISystem()
{
	filesystem_ptr fs(new gui_filesystem("/data/"));

	m_render_device = std::make_shared<gui::ogl_platform::RenderDeviceGL>(fs, 1024);
	m_render = std::make_shared<gui::Renderer>(*m_render_device, fs);

	if(m_system)
		m_system.reset();

	m_system = std::make_shared<System>(*m_render, "default", nullptr, g_log);

	if(m_system)
	{
		//::ShowCursor(FALSE);
		Cursor& cursor = m_system->getCursor();
		cursor.setType("CursorNormal");
		//m_font = m_system->getWindowManager().loadFont("exotibi");
	}
}

void ui_test_application::resetGUISystem()
{
	if(m_render)
		m_render->clearRenderList();

	if(m_system)
		m_system->reset();	
}

void ui_test_application::update()
{
	m_framecount++;
	if(m_system)
	{
		m_system->tick(env.dt);
		m_system->draw();
	}	
}

void ui_test_application::render()
{
	//m_render_device->frame_begin();
	//m_render_device.clear(rgde::math::color::Black);

	if (m_system)
	{		
	//	//m_font->drawText("THE", gui::Rect(20,20,200,200), 1.f);

	//	//gui::Renderer& r = m_system->getRenderer();
	//	//struct vec2 {float x, y;};
	//	//vec2 points[] = 
	//	//{
	//	//	{0,50}, 
	//	//	{70,50}, 
	//	//	{80,90},
	//	//	{110,0},
	//	//	{130,60},
	//	//	{150,50},
	//	//	{260,50},
	//	//};

	//	//gui::Imageset* imageset = m_system->getWindowManager().getImageset("skin");
	//	//const gui::Image* img = imageset->GetImage("Background");

	//	//r.drawLine(*img, (gui::vec2*)points, 7, 1, gui::Rect(0,0,400,400), 0xFFFF0F0F, 7);

		m_system->render();
	}

	//m_render_device->frame_end();
	//m_render_device.present();
}

bool ui_test_application::isFinished() 
{
	/*if(m_framecount >= 50)
	return true;*/

	return false;
}
//
//core::windows::result ui_test_application::wnd_proc(ushort message, uint wparam, long lparam )
//{
//	switch (message)
//	{
//	case WM_CHAR:
//		handleChar((UINT_PTR)wparam);
//		return 0;
//
//	case WM_LBUTTONDOWN:
//		handleMouseButton(EventArgs::Left, EventArgs::Down);
//		return 0;
//
//	case WM_LBUTTONUP:
//		handleMouseButton(EventArgs::Left, EventArgs::Up);
//		return 0;
//
//	case WM_RBUTTONDOWN:
//		handleMouseButton(EventArgs::Right, EventArgs::Down);
//		return 0;
//
//	case WM_RBUTTONUP:
//		handleMouseButton(EventArgs::Right, EventArgs::Up);
//		return 0;
//
//	case WM_MBUTTONDOWN:
//		handleMouseButton(EventArgs::Middle, EventArgs::Down);				
//		return 0;
//
//	case WM_MBUTTONUP:
//		handleMouseButton(EventArgs::Middle, EventArgs::Up);
//		return 0;
//
//	case WM_ACTIVATE:	// Watch For Window Activate Message
//		m_active = !HIWORD(wparam);// Check Minimization State
//		return 0;
//
//	case WM_KEYDOWN:
//		{
//			if ('Q' == wparam || 'q' == wparam || VK_ESCAPE == wparam)
//				exit(0);
//
//			handleKeyboard((UINT_PTR)wparam, EventArgs::Down);
//
//			return 0;
//		}
//
//	case WM_KEYUP:
//		handleKeyboard((UINT_PTR)wparam, EventArgs::Up);
//		return 0;
//
//	case WM_SIZE:
//		//resize_scene(LOWORD(lparam), HIWORD(lparam));
//		return 0;
//
//	case WM_MOUSEWHEEL:
//		{
//			int delta = GET_WHEEL_DELTA_WPARAM(wparam);
//			if(m_system) {
//				gui::event e = {0};
//				e.type = gui::event_mouse | gui::mouse_wheel;
//				e.mouse.delta = delta;
//				m_system->handle_event(e);
//			}
//		}
//		return 0;
//
//	case WM_MOUSEMOVE:
//		if(m_system){
//			gui::event e = {0};
//			e.type = gui::event_mouse | gui::mouse_move;
//			e.mouse.x = LOWORD(lparam);
//			e.mouse.y = HIWORD(lparam);
//			return m_system->handle_event(e);
//		}
//		return 0;
//	}
//	return window::wnd_proc(message, wparam, lparam);
//}

void ui_test_application::handleViewportChange()
{
	if(!m_system) return;

	gui::event e = {0};
	e.type = gui::event_viewport_resize;
	m_system->handle_event(e);
}


bool ui_test_application::handleMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
{
	if(m_system) {
		gui::event e = {0};
		e.type = gui::event_mouse | gui::mouse_button;
		e.type |= (state == EventArgs::Down) ? gui::event_key_down : gui::event_key_up;

		switch(btn) {
			case EventArgs::Left:
				e.mouse.button = gui::button_left;
				break;
			case EventArgs::Middle:
				e.mouse.button = gui::button_middle;
				break;
			case EventArgs::Right:
				e.mouse.button = gui::button_right;
				break;
		}

		return m_system->handle_event(e);
	}
	else
		return false;
}

void ui_test_application::onMousebutton(int button, int action) {
	if (!m_system || button > 2 || action > 1) return;
	int gui_buttons_map[] = { gui::button_left, gui::button_right, gui::button_middle };
	gui::event_type gui_actions_map[] = { gui::event_key_up, gui::event_key_down };

	gui::event e = { 0 };
	e.type = gui::event_mouse | gui::mouse_button;
	e.type |= gui_actions_map[action];
	e.mouse.button = gui_buttons_map[button];
	m_system->handle_event(e);
}

void ui_test_application::onMousepos(int x, int y) {
	mouse_x = x;
	mouse_y = y;
	if (!m_system) return;

	gui::event e = {0};
	e.type = gui::event_mouse | gui::mouse_move;
	e.mouse.x = mouse_x;
	e.mouse.y = mouse_y;
	m_system->handle_event(e);
}

void ui_test_application::onMousewheel(int delta) {
	if (!m_system) return;
	gui::event e = {0};
	e.type = gui::event_mouse | gui::mouse_wheel;
	e.mouse.delta = delta;
	m_system->handle_event(e);
}

void ui_test_application::onKey(int key, int action) {
	if (!m_filename.empty() && key == 294 && action == 1)
	{
		resetGUISystem();
		return;
	}

	if (!m_system) return;
}

void ui_test_application::onChar(int character, int action) {
	if (!m_system) return;
}
//
//bool ui_test_application::handleKeyboard(UINT_PTR key, EventArgs::ButtonState state)
//{
//	if(!m_system) return false;
//
//	if (!m_filename.empty())
//		if((EventArgs::Keys)key == EventArgs::K_F5 && state == EventArgs::Down)
//		{
//			resetGUISystem();
//			return true;
//		}
//
//	gui::event e = {0};
//	e.type = gui::event_keyboard;
//	e.type |= (state == EventArgs::Down) ? gui::event_key_down : gui::event_key_up;
//	e.keyboard.key = (gui::EventArgs::Keys)key;
//	return m_system->handle_event(e);
//}
//
//bool ui_test_application::handleChar(UINT_PTR ch)
//{
//	if(m_system){
//		gui::event e = {0};
//		e.type = gui::event_char;
//		e.text.code = ch;
//		return m_system->handle_event(e);
//		//return m_system->handleChar((unsigned int)ch);
//	}
//	else
//		return false;
//}



void ui_test_application::load(const std::string& xml)
{
	if(!m_system) return;
	gui::base_window* wnd = m_system->loadXml(xml);
}
