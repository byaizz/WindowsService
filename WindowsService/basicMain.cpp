#include "basicMain.h"
#include <windows.h>
#include <fstream>
#include <iostream>

bool g_exitFlag = true;//�˳���ǣ�falseΪ�˳�

void service_thread_func(void *exitFlag)
{
	//�̺߳�����ִ�г���������
	while (*(bool*)exitFlag)
	{
		//ִ������
		std::ofstream ofs("e:\\custom_service.txt", std::ios::app);
		if (!ofs)
		{
			return;
		}
		int i = 0;
		while (i < 10)
		{
			ofs << "�� " << ++i << " ��" << std::endl;
			Sleep(1000);
		}
		ofs.close();

		Sleep(1000);
	}

	return;
}

BasicMain::BasicMain(const char* serviceName, const char* displayName)
	:ServiceBase(serviceName, displayName)
	,m_threadHandle(NULL)
{
}

BasicMain::~BasicMain()
{
}

void BasicMain::OnStart(DWORD argc, TCHAR * argv[])
{
	// Create thread.
	m_threadHandle = new std::thread(service_thread_func, (void *)&g_exitFlag);
}

void BasicMain::OnStop()
{
	g_exitFlag = false;//�˳����
	if (m_threadHandle != NULL)
	{
		m_threadHandle->join();
	}
}
