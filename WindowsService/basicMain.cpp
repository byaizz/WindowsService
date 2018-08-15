#include "basicMain.h"
#include <windows.h>
#include <thread>
#include <fstream>
#include <iostream>

void custom_service()
{
	std::ofstream ofs("\\custom_service.txt", std::ios::app);
	if (!ofs)
	{
		return;
	}
	int i = 10;
	while (i < 100)
	{
		ofs << "第 " << ++i << " 次" << std::endl;
		Sleep(100);
	}
	ofs.close();
}

BasicMain::BasicMain(const char* serviceName, const char* displayName)
	:ServiceBase(serviceName, displayName)
{
}
BasicMain::~BasicMain()
{
}

void BasicMain::service_thread_func(bool deBug)
{
	std::thread main_thread(custom_service);//新线程执行main_service，
	if (deBug)
	{
		main_thread.join();
	}
	else
	{
		main_thread.detach();
	}
}
void BasicMain::OnStart(DWORD argc, TCHAR * argv[])
{
	std::thread service_thread(service_thread_func, false);
	service_thread.detach();
}
