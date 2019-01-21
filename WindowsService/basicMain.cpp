#include <iostream>
#include <process.h>
#include "basicMain.h"

bool g_exitFlag = true;//�˳���ǣ�falseΪ�˳�

unsigned int _stdcall service_thread_func(void *exitFlag)
{
	//�̺߳�����ִ�г���������
	while (*(bool*)exitFlag)
	{
		//ִ������
		std::ofstream ofs("\\custom_service.txt", std::ios::app);
		if (!ofs)
		{
			return;
		}
		int i = 0;
		while (i < 100)
		{
			ofs << "�� " << ++i << " ��" << std::endl;
			Sleep(1000);
		}
		ofs.close();

		Sleep(1000);
	}

	return 0;
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
	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, service_thread_func, (void *)&g_exitFlag, 0, NULL);
}

void BasicMain::OnStop()
{
	g_exitFlag = false;//�˳����
	WaitForSingleObject(m_threadHandle, INFINITE);//�ȴ��߳̽���
	CloseHandle(m_threadHandle);//�ر��߳̾��
}
