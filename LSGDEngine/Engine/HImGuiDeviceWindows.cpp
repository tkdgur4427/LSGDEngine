#include "HEnginePCH.h"
#include "HImGuiDeviceWindows.h"

using namespace lsgd;

HImGuiDeviceWindows::HImGuiDeviceWindows()
{

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

void HImGuiDeviceWindows::Tick()
{
	ImGuiIO& IO = ImGui::GetIO();
	check(IO.Fonts->IsBuilt());

	// setup display size (every frame to accommodate for window resizing)
	RECT Rect;
	::GetClientRect((HWND)Handle, &Rect);
	IO.DisplaySize = ImVec2((float)(Rect.right - Rect.left), (float)(Rect.bottom, Rect.top));

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