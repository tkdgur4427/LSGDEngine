#include "HEnginePCH.h"
#include "HWindowFrameWindows.h"

#include "HImGuiDeviceWindows.h"

HWindowFrameWindows::HWindowFrameWindows()
{

}

HWindowFrameWindows::~HWindowFrameWindows()
{

}

// @todo - temporary setting msg process here (need to move)
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GImGuiDeviceWindows)
	{
		if (GImGuiDeviceWindows->UpdateMessage((void*)hWnd, msg, wParam, lParam))
		{
			return true;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void HWindowFrameWindows::CreateWindowFrame()
{
	WindowClass = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, TEXT("SGDEngine"), NULL };
	RegisterClassEx(&WindowClass);
	Handle = CreateWindowExW(0, WindowClass.lpszClassName, TEXT("SGDEngine"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, WindowClass.hInstance, NULL);
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