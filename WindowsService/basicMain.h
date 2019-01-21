#ifndef BASICMAIN_H
#define BASICMAIN_H

#include "serviceBase.h"
#include <thread>

extern bool g_exitFlag;//�߳��˳���ǣ�falseΪ�˳�
void service_thread_func(void *exitFlag);

class BasicMain : public ServiceBase{
public:
	BasicMain(const char* serviceName, const char* displayName);
	~BasicMain();

protected:
	void OnStart(DWORD argc, TCHAR* argv[]);//��д�����麯�����ڸú���ִ����Ҫ����
	void OnStop();//��д�����麯�����ڸú�����ִ�з���ֹͣ��Ҫ���������
private:
	std::thread *m_threadHandle;//�߳̾��
};

#endif	//BASICMAIN_H