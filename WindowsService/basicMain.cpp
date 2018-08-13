#include "basicMain.h"
#include <fstream>
#include <windows.h>
#include <thread>

void main_service()
{
	std::ofstream ofs("test.txt", std::ios::app);
	if (!ofs)
	{
		MessageBox(NULL, TEXT("���ļ�ʧ��"), NULL, MB_OK);
	}
	MessageBox(NULL, TEXT("���ļ��ɹ�"), NULL, MB_OK);
	int i = 0;
	while (true)
	{
		ofs << "�� " << ++i << " ��" << std::endl;
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
	std::thread main_thread(main_service);//���߳�ִ��main_service��
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
