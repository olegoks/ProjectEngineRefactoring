#include "Window.hpp"
//
#include <chrono>
//


//Window constants
const std::wstring Window::kDefaultTitle{ L"Default window title" };
const std::size_t Window::kDefaultWidth{ 1920 };
const std::size_t Window::kDefaultHeight{ 1080 };
const std::size_t Window::kDefaultX{ 0 };
const std::size_t Window::kDefaultY{ 0 };
const std::wstring Window::kClassName{ L"Main window class name." };

Window::Window()noexcept(true):
	hInstance_{ nullptr },
	params_{ kDefaultWidth, 
	kDefaultHeight, 
	kDefaultX, 
	kDefaultY, 
	kDefaultTitle },
	device_context_{ nullptr },
	self_hWnd_{ nullptr }{}


void Window::PushKeystroke(KeyAction key)noexcept(true) {

	using namespace std::chrono;
	high_resolution_clock::time_point now_time_point = high_resolution_clock::now();
	long long  now = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
	const Keystroke keystroke = Keystroke{ key, now };
	keystrokes_queue_.Push(keystroke);

}

LRESULT CALLBACK Window:: WndProc(HWND hWnd, UINT Message, WPARAM  wParam, LPARAM lParam) {

	Window* this_wnd = reinterpret_cast<Window*>(GetWindowLong(hWnd, 0));

	switch (Message) {

	case WM_KEYDOWN: {


		const unsigned int key = wParam;

		const Action key_action = Action::Down;

		switch (key) {

		case VK_UP: { this_wnd->PushKeystroke(KeyAction{ Key::ArrowUp, key_action }); break; }
		case VK_DOWN: { this_wnd->PushKeystroke(KeyAction{ Key::ArrowDown, key_action }); break; }
		case VK_LEFT: { this_wnd->PushKeystroke(KeyAction{ Key::ArrowLeft, key_action }); break; };
		case VK_RIGHT: { this_wnd->PushKeystroke(KeyAction{ Key::ArrowRight, key_action }); break; }
		case VK_A: { this_wnd->PushKeystroke(KeyAction{ Key::A, key_action }); break; };
		case VK_D: { this_wnd->PushKeystroke(KeyAction{ Key::D, key_action }); break; };
		case VK_S: { this_wnd->PushKeystroke(KeyAction{ Key::S, key_action }); break; };
		case VK_W: { this_wnd->PushKeystroke(KeyAction{ Key::W, key_action }); break; };

		}
		break;
			
	}

	case WM_KEYUP: {

		const unsigned int key = wParam;

		const Action key_action = Action::Up;

		switch (key) {

		case VK_UP: { this_wnd->PushKeystroke(KeyAction{Key::ArrowUp, key_action}); break; }
		case VK_DOWN: { this_wnd->PushKeystroke(KeyAction{ Key::ArrowDown, key_action }); break; }
		case VK_LEFT: { this_wnd->PushKeystroke(KeyAction{ Key::ArrowLeft, key_action }); break; };
		case VK_RIGHT: { this_wnd->PushKeystroke(KeyAction{ Key::ArrowRight, key_action }); break; }
		case VK_A: { this_wnd->PushKeystroke(KeyAction{ Key::A, key_action }); break; };
		case VK_D: { this_wnd->PushKeystroke(KeyAction{ Key::D, key_action }); break; };
		case VK_S: { this_wnd->PushKeystroke(KeyAction{ Key::S, key_action }); break; };
		case VK_W: { this_wnd->PushKeystroke(KeyAction{ Key::W, key_action }); break; };

		}
		break;

	}

	case WM_MOUSEWHEEL: {

		static int wheel_delta = 0;
		wheel_delta += GET_WHEEL_DELTA_WPARAM(wParam);
				
		while (wheel_delta > WHEEL_DELTA) {
					//PushKeystroke(WheelUp);
			wheel_delta -= WHEEL_DELTA;
		}
				
		while (wheel_delta < 0) {
					//PushKeystroke(WheelDown);
			wheel_delta += WHEEL_DELTA;
		}

		break;
	}

	case WM_CLOSE: { DestroyWindow(hWnd);break; }

	case WM_DESTROY: { PostQuitMessage(0);break; }
		
	default: return DefWindowProc(hWnd, Message, wParam, lParam); 
		
	}

}

void Window::PlugHInstance(const HINSTANCE hInstance)const noexcept(true){

	if (hInstance_ == 0)
		hInstance_ = hInstance;

}
		
Window::~Window() {}

void Window::RegisterWindowClass()noexcept(true) {

	WNDCLASSEX wc; //������� ��������� ������ WNDCLASSEX
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = this->WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 4;
	wc.hInstance = hInstance_;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = kClassName.c_str();
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {

		MessageBox(nullptr,
			L"�� ������� ���������������� �����",
			L"������ ����������� ������!",
			MB_OK);

	}

}

void Window::Create()noexcept(true){
			
			self_hWnd_ = CreateWindowEx(WS_EX_TOPMOST, kClassName.c_str(),
				params_.title_.c_str(),
				WS_OVERLAPPEDWINDOW,//WS_POPUP,
				params_.x_,
				params_.y_, 
				params_.width_, 
				params_.height_, 
				HWND(NULL), 
				HMENU(NULL), 
				(HINSTANCE)hInstance_, 
				NULL);
			
			if (!self_hWnd_) {
	
				MessageBox(NULL, L"�� ���������� ������� ����!", L"������", MB_OK);

			}

			//���-�� ����� �������))
			SetWindowLong(self_hWnd_, 0, (LONG)this);

			SetProcessDPIAware();//������� ��� ������� (SetProcessDPIAware), �� ��������� �������, ��� ��������� ������ ���������� ����� ��� ��������� ���������������� ��� ������� ��������� DPI (����� �� ����). ���� �� �� ��������� ���� ����, �� ��������� ������ ���������� ����� ��������� ������� ��� ������� ��������� DPI.
			device_context_ = GetDC(self_hWnd_);

		}

		void Window::Show()const noexcept(true)
		{
			ShowWindow(self_hWnd_, SW_MAXIMIZE);
		}

		WPARAM Window::StartMessageLoop()const noexcept(true){
			
			/*
			typedef struct tagMSG {
				HWND hwnd; // ���������� ����, �������� ���������� ���������
				UINT message; // ����� (������������� ) ���������
				WPARAM wParam; // �������� ��������� wParam
				LPARAM lParam; //�������� ��������� lParam
				DWORD time; // ����� �������� ���������
				POINT pt; //������� �������(� �������� �����������) � ������
				//�������� ���������
			} MSG;
			*/
	
			MSG msg; // ����� ��������� ��������� MSG ��� ��������� ���������

			msg = { 0 }; // ������������� ��������� ��������� 

			while (GetMessage(&msg, NULL, NULL, NULL) != 0) {
					
					TranslateMessage(&msg);
					DispatchMessage(&msg);

			}

			// BOOL GetMessage(
			// MSG lpMsg, - ����� ��������� ���� ��������� ���������
			// HWND hWnd, - �������� ����� ��������� ����� NULL, 
			// UINT wMsgFilterMin, - ���������� ������������� �������� ������ ���������� �������� ���������, ������� ����� ���������.
			// UINT wMsgFilterMax - ���������� ������������� �������� ������ �������� �������� ���������, ������� ����� ���������.
			// *���� wMsgFilterMin � wMsgFilterMax �������� ��� ��������, ������� GetMessage ���������� ��� ��������� ��������� (�� ���� ������� ���������� � ��������� �������� �� �����������).
			
			//������� TranslateMessage ��������� ��������� ����������� ������ � ���������� ���������. 
			//���������� ��������� ���������� � ������� ��������� ����������� ������ ��� ��������� � ��������� ���, 
			//����� ����� ������� ������� GetMessage ��� PeekMessage.

			//������� DispatchMessage ������������ ��������� ������� ��������� WndProc.
			return msg.wParam;

		}

		void Window::ShowFrame(const FrameHost& frame)noexcept(true) {
			
			using namespace std::chrono;
			using time_point = high_resolution_clock::time_point;
			using duration = high_resolution_clock::duration;

			static time_point last_time_point = high_resolution_clock::now();

			duration render_frame_duration = high_resolution_clock::now() - last_time_point;

			size_t render_duration_ms = duration_cast<milliseconds>(render_frame_duration).count();
			size_t current_fps = 1000.0f / static_cast<float>(render_duration_ms);

			if (current_fps > 100) { current_fps = 99; }
				else
					if (current_fps < 0) { current_fps = 0; };

			//std::wstring fps_wstr = ConvertStringToWstring(std::to_string(current_fps));
			SetWindowTextA(
				self_hWnd_,
				std::to_string(current_fps).c_str()
			);
			//TextOut(device_context_, 5, 5, fps_wstr.c_str(), 2);

			SetDIBitsToDevice(
					device_context_,
					0,
					0,
					params_.width_, 
					params_.height_,
					0,
					0,
					0,
					params_.height_,
					static_cast<const void*>(frame),
					&frame.Info(),
					DIB_PAL_COLORS
			);

			last_time_point = high_resolution_clock::now();
			
			//first_time_point_count_fps_ = high_resolution_clock::now();*/

				//int SetDIBitsToDevice(
				//	HDC hdc,                 // ���������� DC
				//	int XDest,               // x-�����. �������� ������ ���� ���������
				//	int YDest,               // y-�����. �������� ������ ���� ���������
				//	DWORD dwWidth,           // ������ �������������� ���������
				//	DWORD dwHeight,          // ������ �������������� ���������
				//	int XSrc,                // x-�����. ������� ������ ���� ���������
				//	int YSrc,                // y-�����. ������� ������ ���� ���������
				//	UINT uStartScan,         // ������ ������ ��������� � �������
				//	UINT cScanLines,         // ����� ����� ���������
				//	CONST VOID * lpvBits,     // ������ ����� DIB
				//	CONST BITMAPINFO * lpbmi, // ���������� � �������� �������
				//	UINT fuColorUse          // RGB ��� ������� �������
				//);
			
		}
