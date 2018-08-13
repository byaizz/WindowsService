#ifndef BASICMAIN_H
#define BASICMAIN_H

#include "serviceBase.h"

void main_service();

class BasicMain : public ServiceBase{
public:
	BasicMain(const char* serviceName, const char* displayName);
	~BasicMain();

	static void service_thread_func(bool deBug);
	void OnStart(DWORD argc, TCHAR* argv[]);
};

#endif	//BASICMAIN_H