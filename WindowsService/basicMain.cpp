#include "basicMain.h"
#include <windows.h>
#include <thread>
#include <fstream>
#include <iostream>

BasicMain::BasicMain(const char* serviceName, const char* displayName)
	:ServiceBase(serviceName, displayName)
{
}

BasicMain::~BasicMain()
{
}

void BasicMain::service_thread_func()
{
	std::ofstream ofs("\\custom_service.txt", std::ios::app);
	if (!ofs)
	{
		return;
	}
	int i = 0;
	while (i < 100)
	{
		ofs << "µÚ " << ++i << " ´Î" << std::endl;
		Sleep(1000);
	}
	ofs.close();
}

void BasicMain::OnStart(DWORD argc, TCHAR * argv[])
{
	std::thread service_thread(service_thread_func);
	service_thread.detach();
}
