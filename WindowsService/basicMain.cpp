#include <iostream>
#include <process.h>
#include "basicMain.h"

bool g_exitFlag = true;//退出标记，false为退出

unsigned int _stdcall service_thread_func(void *exitFlag)
{
	//线程函数，执行程序主任务
	while (*(bool*)exitFlag)
	{
		//执行任务
		std::ofstream ofs("\\custom_service.txt", std::ios::app);
		if (!ofs)
		{
			return;
		}
		int i = 0;
		while (i < 100)
		{
			ofs << "第 " << ++i << " 次" << std::endl;
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
	g_exitFlag = false;//退出标记
	WaitForSingleObject(m_threadHandle, INFINITE);//等待线程结束
	CloseHandle(m_threadHandle);//关闭线程句柄
}
