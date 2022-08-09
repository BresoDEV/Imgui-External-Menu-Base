#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") 
#define DIRECTINPUT_VERSION 0x0800
#include "main.h"

#include <stdio.h> 

#include <windows.h>
#include <tlhelp32.h>
#include <string>




using namespace std;



class Teste
{
public:
	static bool Injetar(char* procName, char* dllName)
	{
		DWORD processID = ID_Processo_pelo_nome(procName);
		return Injetar(processID, dllName);
	}

	static bool Injetar(DWORD processID, char* relativeDllName)
	{
		if (processID == 0)
			return false;
		HANDLE Processo = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
		if (Processo == 0)
			return false;
		char ParasitaDLL[MAX_PATH];
		GetFullPathNameA(relativeDllName, MAX_PATH, ParasitaDLL, NULL);
		LPVOID CarregarLibs = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
		LPVOID Variavel_remota_teste = VirtualAllocEx(Processo, NULL, strlen(ParasitaDLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		WriteProcessMemory(Processo, Variavel_remota_teste, ParasitaDLL, strlen(ParasitaDLL), NULL);
		CreateRemoteThread(Processo, NULL, NULL, (LPTHREAD_START_ROUTINE)CarregarLibs, (LPVOID)Variavel_remota_teste, NULL, NULL);
		CloseHandle(Processo);
		return true;
	}

	static DWORD ID_Processo_pelo_nome(const char* ProcName)
	{
		HANDLE ABACADABRA = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (ABACADABRA == INVALID_HANDLE_VALUE)
			return 0;
		PROCESSENTRY32 Processo;
		Processo.dwSize = sizeof(PROCESSENTRY32);
		BOOL retval = Process32First(ABACADABRA, &Processo);
		while (retval)
		{
			if (!strcmp(Processo.szExeFile, ProcName))
			{
				CloseHandle(ABACADABRA);
				return Processo.th32ProcessID;
			}
			retval = Process32Next(ABACADABRA, &Processo);
		}
		CloseHandle(ABACADABRA);
		return 0;
	}
};


enum Submenus
{
	Fechado,
	Principal,
};


int menuIndex = 0;
int menuIndexanterior = 1;


void AddTitle(const char* Texto, int atual, int antMenu)
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
	ImGui::Button(Texto, ImVec2(WindowWidth, 45));
	ImGui::PopStyleColor(3);
	ImGui::PopID();
	menuIndexanterior = antMenu;
	menuIndex = atual;
}

void AddSubmenuOption(const char* Texto, int atual, int ProxMenu)
{
	if (ImGui::Button(Texto, ImVec2(WindowWidth, 25)))
	{
		menuIndexanterior = atual;
		menuIndex = ProxMenu;
	}
}

void AddOption(const char* Opcao, void (func)() = NULL)
{
	if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
	{
		if (func != NULL)
			func();
	}
}


void Controles()
{
	
	if (GetAsyncKeyState(VK_INSERT) == -32767)
	{
		if (menuIndex == 0)
			menuIndex = 1;
		else
			menuIndex = 0;
		Sleep(200);
	}
	if (GetAsyncKeyState(VK_BACK) == -32767)
	{
		menuIndex = menuIndexanterior;
	}
}


void Loop()
{
	
}


 



int main(int, char**)
{

	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, AppClass, NULL };
	RegisterClassEx(&wc);
	hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, AppClass, AppName, WS_POPUP, (desktop.right / 2) - (WindowWidth / 2), (desktop.bottom / 2) - (WindowHeight / 2), WindowWidth, WindowHeight, 0, 0, wc.hInstance, 0);

	//SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);

	if (CreateDeviceD3D(hwnd) < 0)
	{
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	DefaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	static bool open = true;
	 
	io.ConfigFlags |= ImGuiWindowFlags_NoResize;
	//DWORD dwFlag =  0;
	DWORD dwFlag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	char somelogin[25] = "";
	while (msg.message != WM_QUIT)
	{
		
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		//-------------------
		Loop();
		Controles();
		//------------------
		if (!open) ExitProcess(EXIT_SUCCESS);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame(); 
		ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		{

			switch (menuIndex)
			{
			case Fechado: break; 
				ImGui::Begin("BresoDEV Menu Base", &open, dwFlag);
				ImGui::End();
			case Principal: 
			{
				
				ImGui::Begin(" ", &open, dwFlag);
				AddTitle("BresoDEV Menu Base", 1,0);
				 
				AddSubmenuOption("Subbbb", Principal, 2);
				if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(0.8);
				}if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(0.5);
				}if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(0.4);
				}
				ImGui::End();
			}
			break;
			case 2:
			{
				ImGui::Begin(" ", &open, dwFlag);
				AddTitle("BresoDEV Menu Base",2,1);

				AddSubmenuOption("Subbb 3", 2, 3);
				if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(1.0);
				}if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(1.2);
				}if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(0.9);
				}
				ImGui::End();
			}
			break;
			case 3:
			{
				ImGui::Begin(" ", &open, dwFlag);
				AddTitle("BresoDEV Menu Base",3, 2);

				
				if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(1.2);
				}if (ImGui::Button("Submenu 2", ImVec2(WindowWidth, 25)))
				{
					ImGui::SetWindowFontScale(0.9);
				}
				ImGui::End();
			}
			break;
			}


		}
		

		ImGui::EndFrame();

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{

			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) ResetDevice();

	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}
