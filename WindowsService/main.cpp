#include "serviceBase.h"

#include <iostream>
using std::cout;
using std::endl;

#define SERVICE_NAME				"serviceTest"				//internal name of service
#define SERVICE_DISPLAY_NAME		"serviceTest_by"			//displayed name of service
#define SERVICE_START_TYPE			SERVICE_DEMAND_START		//service start options
#define SERVICE_DEPENFENCIES		""
#define	SERVICE_ACOUNT				"NT AUTHORITY\\LocalService"
#define SERVICE_PASSWORD			NULL

void ShowHelp()
{
	cout << "-i	to install the service.\n"
		"-r	to remove the service.\n" << endl;
}
int main(int argc, char *argv[])
{
	if (argc < 1) return 0;
	if (argc > 2)
	{
		ShowHelp();
	}

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
			break;
		case 'r':
		case 'R':
			break;
		default:
			ShowHelp();
			break;
		}
	}
	else
	{
		//start service
	}

	return 0;
}