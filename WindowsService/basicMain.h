#ifndef BASICMAIN_H
#define BASICMAIN_H

#include "serviceBase.h"

void custom_service();

class BasicMain : public ServiceBase{
public:
	BasicMain(const char* serviceName, const char* displayName);
	~BasicMain();

	static void service_thread_func();
	void OnStart(DWORD argc, TCHAR* argv[]);
};

#endif	//BASICMAIN_H