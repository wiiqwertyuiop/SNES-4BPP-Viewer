#include<SFML/Graphics.hpp>
#include <windows.h>
#include <shobjidl.h> 
#include "RenderSFML.h"

void KeyUP(WPARAM, mySFML&);
void KeyDOWN(WPARAM, mySFML&);
std::string OpenFileDiag(bool);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Test";

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		"Test",                     // Window class
		"SNES GFX Test",    // Window text
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_SIZEBOX,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 815, 670,

		0,       // Parent window    
		0,       // Menu
		hInstance,  // Instance handle
		0        // Additional application data
	);

	if (!hwnd) return 0;

	DWORD Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;

	HWND View1 = CreateWindow("STATIC", 0, Style, 0, 10, 800, 600, hwnd, 0, hInstance, 0);

	sf::RenderWindow window(View1);
	mySFML SFML;

	if (SFML.Error != "") {
		MessageBox(0, SFML.Error.c_str(), "Error!", MB_OK | MB_ICONERROR);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	// Run the message loop.

	MSG msg = { };

	while(GetMessage(&msg, 0, 0, 0))
	{

		if (msg.message == WM_KEYDOWN || msg.message == WM_CHAR) {
			KeyDOWN(msg.wParam, SFML);
		}
		else if(msg.message == WM_KEYUP)
		{
			KeyUP(msg.wParam, SFML);
		} 
		else if (msg.message ==  WM_COMMAND)

			switch (LOWORD(msg.wParam)) {

			case 1:
				SFML.OpenGFXFile( OpenFileDiag(LOWORD(msg.wParam) - 1) );
				if (SFML.Error != "") MessageBox(0, SFML.Error.c_str(), "Error!", MB_OK | MB_ICONERROR);
				break;

			case 2:
				SFML.OpenPalFile( OpenFileDiag( LOWORD(msg.wParam)-1 ) );
				if (SFML.Error != "") MessageBox(0, SFML.Error.c_str(), "Error!", MB_OK | MB_ICONERROR);
				break;

			case 3:
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				break;				

			
			}

		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		SFML.Update(window);
		
	}

	DestroyWindow(hwnd);
	UnregisterClass("Test", hInstance);
	return 0;
}

void KeyDOWN(WPARAM wParam, mySFML& SFML) {
	if (wParam == VK_PRIOR) {
		SFML.PageGFX(0);
	}
	else if (wParam == VK_NEXT) {
		SFML.PageGFX(1);
	}
	else if (wParam == 0x73 || wParam == 0X53) {
		SFML.SPressed(1);
	}

}

void KeyUP(WPARAM wParam, mySFML& SFML) {

	if (wParam == 0x73 || wParam == 0X53) {
		SFML.SPressed(0);
	}
	else if (wParam == VK_UP) {
		SFML.HandlePalCursor(0);
	}
	else if (wParam == VK_DOWN) {
		SFML.HandlePalCursor(1);
	}

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_DESTROY:

		PostQuitMessage(0);
		break;		

	case WM_CREATE:

		HMENU hMenubar;
		HMENU hMenu;

		hMenubar = CreateMenu();
		hMenu = CreateMenu();

		AppendMenuW(hMenu, MF_STRING, 1, L"&Open GFX File");
		AppendMenuW(hMenu, MF_STRING, 2, L"&Open .pal File");
		AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenuW(hMenu, MF_STRING, 3, L"&Quit");

		AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File  ");
		SetMenu(hwnd, hMenubar);
		break;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



std::string OpenFileDiag(bool FileType) {
	
	OPENFILENAME ofn;
	
	char file_name[100];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(file_name);

	if(!FileType) ofn.lpstrFilter = "Graphics File (.bin)\0*.bin\0All files\0*.*";
	else ofn.lpstrFilter = "Palette File (.pal)\0*.pal\0All files\0*.*";

	ofn.nFilterIndex = 1;

	GetOpenFileName(&ofn);

	return file_name;

}