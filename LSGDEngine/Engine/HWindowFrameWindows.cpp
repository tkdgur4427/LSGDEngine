#include "HEnginePCH.h"
#include "HWindowFrameWindows.h"

HWindowFrameWindows::HWindowFrameWindows()
{

}

HWindowFrameWindows::~HWindowFrameWindows()
{

}

// @todo - temporary setting msg process here (need to move)
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void HWindowFrameWindows::CreateWindowFrame()
{
	WindowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "SGDEngine", NULL };
	RegisterClassEx(&WindowClass);
	Handle = CreateWindow(WindowClass.lpszClassName, "SGDEngine", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, WindowClass.hInstance, NULL);
}

void HWindowFrameWindows::DestroyWindowFrame()
{
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
	DestroyWindow(Handle);
}

void HWindowFrameWindows::ShowWindowFrame()
{
	ShowWindow(Handle, SW_SHOWDEFAULT);
	UpdateWindow(Handle);
}

bool HWindowFrameWindows::UpdateMessage()
{
	MSG Message;
	ZeroMemory(&Message, sizeof(MSG));

	if (PeekMessage(&Message, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		return true;
	}

	return false;
}