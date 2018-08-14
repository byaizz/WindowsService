#include "basicMain.h"
#include <fstream>
#include <windows.h>
#include <thread>
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
		std::cout << i << std::endl;
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
	std::ofstream ofs("\\OnStart.txt", std::ios::app);
	if (!ofs)
	{
		printf("on start---\n");
		return;
	}
	int i = 0;
	while (i < 100)
	{
		ofs << "第 " << ++i << " 次" << std::endl;
		std::cout << i << std::endl;
		Sleep(100);
	}
	ofs.close();
	std::thread service_thread(service_thread_func, false);
	service_thread.detach();
}
