#include "basicMain.h"
#include <fstream>
#include <windows.h>
#include <thread>

void main_service()
{
	std::ofstream ofs("test.txt", std::ios::app);
	if (!ofs)
	{
		MessageBox(NULL, TEXT("打开文件失败"), NULL, MB_OK);
	}
	MessageBox(NULL, TEXT("打开文件成功"), NULL, MB_OK);
	int i = 0;
	while (true)
	{
		ofs << "第 " << ++i << " 次" << std::endl;
	}
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
	std::thread main_thread(main_service);//新线程执行main_service，
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
