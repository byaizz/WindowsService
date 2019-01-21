#ifndef BASICMAIN_H
#define BASICMAIN_H

#include "serviceBase.h"

extern bool g_exitFlag;//线程退出标记，false为退出
unsigned int _stdcall service_thread_func(void *exitFlag);//线程入口函数

class BasicMain : public ServiceBase{
public:
	BasicMain(const char* serviceName, const char* displayName);
	~BasicMain();

protected:
	void OnStart(DWORD argc, TCHAR* argv[]);//重写基类虚函数，在该函数执行主要任务
	void OnStop();//重写基类虚函数，在该函数中执行服务停止需要处理的事项
private:
	HANDLE m_threadHandle;//线程句柄
};

#endif	//BASICMAIN_H