#include "basicMain.h"
#include <iostream>

using std::cout;
using std::endl;

#define SERVICE_NAME				"serviceTest"				//internal name of service
#define SERVICE_DISPLAY_NAME		"serviceTest_by"			//displayed name of service

void ShowHelp()
{
	cout << "-i	to install the service.\n"
		"-u	to uninstall the service.\n" << endl;
	exit(0);
}
int main(int argc, char *argv[])
{
	if (argc < 1) return 0;
	if (argc > 2)
	{
		ShowHelp();
	}

	BasicMain service(SERVICE_NAME, SERVICE_DISPLAY_NAME);

	if (argc == 2)
	{
		if (argv[1][0] != '-')
		{
			ShowHelp();
		}
		switch (argv[1][1])
		{
		case 'i':
		case 'I':
			printf("install.\n");
			service.InstallService();
			break;
		case 'u':
		case 'U':
			printf("uninstall.\n");
			service.UninstallService();
			break;
		case 'd':
		case 'D':
			printf("deBug run.\n");
			BasicMain::service_thread_func();
			break;
		case 's':
		case 'S':
			printf("start service.\n");
			service.StartInstalledService();
			break;
		default:
			ShowHelp();
			break;
		}
	}
	else
	{
		service.Run();//start service
	}

	return 0;
}