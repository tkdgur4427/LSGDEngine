#include "HEnginePCH.h"
#include "HImGuiDeviceWindows.h"

using namespace lsgd;

lsgd::HImGuiDeviceWindows* GImGuiDeviceWindows = nullptr;

HImGuiDeviceWindows::HImGuiDeviceWindows()
{
	GImGuiDeviceWindows = this;
}

HImGuiDeviceWindows::~HImGuiDeviceWindows()
{

}

bool HImGuiDeviceWindows::Initialize(void* InHandle)
{
	if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&TicksPerSecond))
		return false;
	if (!::QueryPerformanceCounter((LARGE_INTEGER*)&Time))
		return false;

	// setup back-end capabilities flags
	Handle = InHandle;
	ImGuiIO& IO = ImGui::GetIO();
	IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;	// we can honor GetMouseCursor() values (optional)
	IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;	// we can honor io.WantSetMousePos requests (optional, rarely used)
	IO.BackendPlatformName = "imgui_impl_win32";
	IO.ImeWindowHandle = (HWND)Handle;

	// keyboard mapping
	IO.KeyMap[ImGuiKey_Tab] = VK_TAB;
	IO.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	IO.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	IO.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	IO.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	IO.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	IO.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	IO.KeyMap[ImGuiKey_Home] = VK_HOME;
	IO.KeyMap[ImGuiKey_End] = VK_END;
	IO.KeyMap[ImGuiKey_Insert] = VK_INSERT;
	IO.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	IO.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	IO.KeyMap[ImGuiKey_Space] = VK_SPACE;
	IO.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	IO.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	IO.KeyMap[ImGuiKey_A] = 'A';
	IO.KeyMap[ImGuiKey_C] = 'C';
	IO.KeyMap[ImGuiKey_V] = 'V';
	IO.KeyMap[ImGuiKey_X] = 'X';
	IO.KeyMap[ImGuiKey_Y] = 'Y';
	IO.KeyMap[ImGuiKey_Z] = 'Z';

	return true;
}

void HImGuiDeviceWindows::UpdateMousePos()
{
	ImGuiIO& IO = ImGui::GetIO();

	// set os mouse position if requested 
	//	- (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	if (IO.WantSetMousePos)
	{
		POINT Pos = { (int32)IO.MousePos.x, (int32)IO.MousePos.y };
		::ClientToScreen((HWND)Handle, &Pos);
		::SetCursorPos(Pos.x, Pos.y);
	}

	// set mouse position
	IO.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	POINT Pos;
	if (HWND ActiveWindow = ::GetForegroundWindow())
	{
		if (ActiveWindow == (HWND)Handle || ::IsChild(ActiveWindow, (HWND)Handle))
		{
			if (::GetCursorPos(&Pos) && ::ScreenToClient((HWND)Handle, &Pos))
			{
				IO.MousePos = ImVec2((float)Pos.x, (float)Pos.y);
			}
		}
	}
}

bool HImGuiDeviceWindows::UpdateMouseCursor()
{
	ImGuiIO& IO = ImGui::GetIO();
	if (IO.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return false;

	ImGuiMouseCursor ImGuiCursor = ImGui::GetMouseCursor();
	if (ImGuiCursor == ImGuiMouseCursor_None || IO.MouseDrawCursor)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		::SetCursor(NULL);
	}
	else
	{
		// Show OS mouse cursor
		LPTSTR Win32Cursor = IDC_ARROW;
		switch (ImGuiCursor)
		{
		case ImGuiMouseCursor_Arrow:        Win32Cursor = IDC_ARROW; break;
		case ImGuiMouseCursor_TextInput:    Win32Cursor = IDC_IBEAM; break;
		case ImGuiMouseCursor_ResizeAll:    Win32Cursor = IDC_SIZEALL; break;
		case ImGuiMouseCursor_ResizeEW:     Win32Cursor = IDC_SIZEWE; break;
		case ImGuiMouseCursor_ResizeNS:     Win32Cursor = IDC_SIZENS; break;
		case ImGuiMouseCursor_ResizeNESW:   Win32Cursor = IDC_SIZENESW; break;
		case ImGuiMouseCursor_ResizeNWSE:   Win32Cursor = IDC_SIZENWSE; break;
		case ImGuiMouseCursor_Hand:         Win32Cursor = IDC_HAND; break;
		}
		::SetCursor(::LoadCursor(NULL, Win32Cursor));
	}
	return true;
}

int64 HImGuiDeviceWindows::UpdateMessage(void* InHandle, uint32 InMSG, uint32 InWParam, uint32 InLParam)
{
	HWND Handle = (HWND)InHandle;

	if (ImGui::GetCurrentContext() == nullptr)
	{
		return 0;
	}

	ImGuiIO& IO = ImGui::GetIO();
	switch (InMSG)
	{
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	{
		int32 Button = 0;
		if (InMSG == WM_LBUTTONDOWN || InMSG == WM_LBUTTONDBLCLK) { Button = 0; }
		if (InMSG == WM_RBUTTONDOWN || InMSG == WM_RBUTTONDBLCLK) { Button = 1; }
		if (InMSG == WM_MBUTTONDOWN || InMSG == WM_MBUTTONDBLCLK) { Button = 2; }
		if (InMSG == WM_XBUTTONDOWN || InMSG == WM_XBUTTONDBLCLK) { Button = (GET_XBUTTON_WPARAM(InWParam) == XBUTTON1) ? 3 : 4; }
		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
			::SetCapture(Handle);
		IO.MouseDown[Button] = true;
		return 0;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	{
		int32 Button = 0;
		if (InMSG == WM_LBUTTONUP) { Button = 0; }
		if (InMSG == WM_RBUTTONUP) { Button = 1; }
		if (InMSG == WM_MBUTTONUP) { Button = 2; }
		if (InMSG == WM_XBUTTONUP) { Button = (GET_XBUTTON_WPARAM(InWParam) == XBUTTON1) ? 3 : 4; }
		IO.MouseDown[Button] = false;
		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == Handle)
			::ReleaseCapture();
		return 0;
	}
	case WM_MOUSEWHEEL:
		IO.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(InWParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_MOUSEHWHEEL:
		IO.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(InWParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (InWParam < 256)
			IO.KeysDown[InWParam] = 1;
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (InWParam < 256)
			IO.KeysDown[InWParam] = 0;
		return 0;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (InWParam > 0 && InWParam < 0x10000)
			IO.AddInputCharacter((unsigned short)InWParam);
		return 0;
	case WM_SETCURSOR:
		if (LOWORD(InLParam) == HTCLIENT && UpdateMouseCursor())
			return 1;
		return 0;
	}
	return 1;
}

void HImGuiDeviceWindows::Tick()
{
	ImGuiIO& IO = ImGui::GetIO();
	check(IO.Fonts->IsBuilt());

	// setup display size (every frame to accommodate for window resizing)
	RECT Rect;
	::GetClientRect((HWND)Handle, &Rect);
	IO.DisplaySize = ImVec2((float)(Rect.right - Rect.left), (float)(Rect.bottom - Rect.top));

	// setup time step
	int64 CurrentTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	IO.DeltaTime = (float)(CurrentTime - Time) / TicksPerSecond;
	Time = CurrentTime;

	// read keyboard modifier inputs
	IO.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
	IO.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
	IO.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
	IO.KeySuper = false;
	// io.KeysDown[], io.MousePos, io.MouseDown[], io.MouseWheel: filled by the WndProc handler below.

	// update os mouse position
	UpdateMousePos();

	// update os mouse cursor with the cursor requested by imgui
	ImGuiMouseCursor MouseCursor = IO.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
	if (LastMouseCursor != MouseCursor)
	{
		LastMouseCursor = MouseCursor;
		UpdateMousePos();
	}
}