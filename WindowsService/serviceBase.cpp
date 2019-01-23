#include "serviceBase.h"
#include "assert.h"
#include <stdio.h>

#pragma region <Static Member>
// Initialize the singleton service instance.
ServiceBase *ServiceBase::m_service = NULL;
#pragma endregion

#pragma region <Public>
//   FUNCTION: CServiceBase::CServiceBase(PWSTR, BOOL, BOOL, BOOL)
//
//   PURPOSE: The constructor of CServiceBase. It initializes a new instance 
//   of the CServiceBase class. The optional parameters (fCanStop, 
///  fCanShutdown and fCanPauseContinue) allow you to specify whether the 
//   service can be stopped, paused and continued, or be notified when system 
//   shutdown occurs.
//
//   PARAMETERS:
//   * pszServiceName - the name of the service
//   * fCanStop - the service can be stopped
//   * fCanShutdown - the service is notified when system shutdown occurs
//   * fCanPauseContinue - the service can be paused and continued
ServiceBase::ServiceBase(const char *pszServiceName, const char *pszDisplayName)
{
	//service name must be a valid string and cannot be NULL
	strcpy_s(m_serviceName, pszServiceName);
	strcpy_s(m_displayName, pszDisplayName);

	m_statusHandle = NULL;
	m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_status.dwCurrentState = SERVICE_STOPPED;
	m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	m_status.dwWin32ExitCode = NO_ERROR;
	m_status.dwServiceSpecificExitCode = 0;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 0;
}

//   FUNCTION: CServiceBase::~CServiceBase()
//
//   PURPOSE: The virtual destructor of CServiceBase. 
ServiceBase::~ServiceBase()
{
}

//install service
bool ServiceBase::InstallService()
{
	//get the executable file path
	TCHAR szFilePath[MAX_PATH];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);

	//open service control manage
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		MessageBox(NULL, TEXT("Couldn't open service manage"), TEXT(m_serviceName), MB_OK);
		return false;
	}

	//install the service into SCM by calling CreateService
	SC_HANDLE hService = CreateService(hSCM,
		TEXT(m_serviceName),
		TEXT(m_displayName),
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,//开机自启动
		SERVICE_ERROR_NORMAL,
		szFilePath,
		NULL,
		NULL,
		NULL,//TEXT(""),
		NULL,
		NULL
	);

	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		MessageBox(NULL, TEXT("couldn't create service"), TEXT(m_serviceName), MB_OK);
		return false;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return true;
}

//uninstall service
bool ServiceBase::UninstallService()
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		MessageBox(NULL, TEXT("couldn't open service manager"), TEXT(m_serviceName), MB_OK);
		return false;
	}

	SC_HANDLE hService = OpenService(hSCM, TEXT(m_serviceName), SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		MessageBox(NULL, TEXT("couldn't open service"), TEXT(m_serviceName), MB_OK);
		return false;
	}

	SERVICE_STATUS status;
	if (ControlService(hService, SERVICE_CONTROL_STOP, &status))
	{
		printf("Stopping %s.\n", m_displayName);
		Sleep(1000);

		while (QueryServiceStatus(hService, &status))
		{
			if (status.dwCurrentState == SERVICE_STOP_PENDING)
			{
				printf("service stop pending...\n");
				Sleep(1000);
			}
			else break;
		}

		if (status.dwCurrentState == SERVICE_STOPPED)
		{
			printf("%s is stopped.\n", m_displayName);
		}
		else
		{
			printf("%s failed to stop.\n", m_displayName);
		}
	}

	//delete service
	bool bDelete = DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	if (!bDelete)
	{
		MessageBox(NULL, TEXT("service couldn't be deleted"), TEXT(m_serviceName), MB_OK);
		return false;
	}
	return true;
}

//start service
bool ServiceBase::StartInstalledService()
{
	bool bResult = false;

	// open service control manager
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return bResult;
	}

	// get service handle
	SC_HANDLE hService = OpenService(hSCM, m_serviceName, SC_MANAGER_ALL_ACCESS);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		MessageBox(NULL, TEXT("Couldn't open service"), TEXT(m_serviceName), MB_OK);
		return bResult;
	}

	// query service status
	SERVICE_STATUS	service_status; 
	if (QueryServiceStatus(hService, &service_status))
	{
		if (service_status.dwCurrentState == SERVICE_RUNNING)
		{
			bResult = TRUE;
		}
		// service is not running, start service
		else if (StartService(hService, 0, NULL))
		{
			bResult = TRUE;
		}
		else
		{
			DWORD nError = GetLastError();
			printf("service start fail. %d\n", nError);
		}
	}
	else
	{
		DWORD nError = GetLastError();
		printf("service status query fail. %d\n", nError);
	}

	printf("now the service is started.\n");
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return bResult;
}

//   FUNCTION: CServiceBase::Run(CServiceBase &)
//
//   PURPOSE: Register the executable for a service with the Service Control 
//   Manager (SCM). After you call Run(ServiceBase), the SCM issues a Start 
//   command, which results in a call to the OnStart method in the service. 
//   This method blocks until the service has stopped.
//
//   PARAMETERS:
//   * service - the reference to a CServiceBase object. It will become the 
//     singleton service instance of this service application.
//
//   RETURN VALUE: If the function succeeds, the return value is TRUE. If the 
//   function fails, the return value is FALSE. To get extended error 
//   information, call GetLastError.
bool ServiceBase::RunService(ServiceBase *service)
{
	if (service == NULL)
	{
		return false;
	}
	m_service = service;

	SERVICE_TABLE_ENTRY serviceTable[] = 
	{
		{ TEXT(m_service->m_serviceName), (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	// Connects the main thread of a service process to the service control 
	// manager, which causes the thread to be the service control dispatcher 
	// thread for the calling process. This call returns when the service has 
	// stopped. The process should simply terminate when the call returns.
	return StartServiceCtrlDispatcher(serviceTable);
}

bool ServiceBase::Run()
{
	return RunService(this);
}

//   FUNCTION: CServiceBase::Stop()
//
//   PURPOSE: The function stops the service. It calls the OnStop virtual 
//   function in which you can specify the actions to take when the service 
//   stops. If an error occurs, the error will be logged in the Application 
//   event log, and the service will be restored to the original state.
void ServiceBase::Stop()
{
	DWORD dwOriginalState = m_status.dwCurrentState;
	try
	{
		LogEvent("service stop pending.");
		//Tell SCM that the service is stopping
		SetServiceStatus(SERVICE_STOP_PENDING);
		//Perform service-specific stop operations
		OnStop();
		//Tell SCM that the service is stopped
		SetServiceStatus(SERVICE_STOPPED);
		LogEvent("service stopped.");
	}
	catch (...)
	{
		// Log the error.
		LogEvent("Service failed to stop.");
		// Set the original service status.
		SetServiceStatus(dwOriginalState);
	}

}
#pragma endregion

#pragma region <Protected>
//   FUNCTION: CServiceBase::OnStart(DWORD, PWSTR *)
//
//   PURPOSE: When implemented in a derived class, executes when a Start 
//   command is sent to the service by the SCM or when the operating system 
//   starts (for a service that starts automatically). Specifies actions to 
//   take when the service starts. Be sure to periodically call 
//   CServiceBase::SetServiceStatus() with SERVICE_START_PENDING if the 
//   procedure is going to take long time. You may also consider spawning a 
//   new thread in OnStart to perform time-consuming initialization tasks.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
void ServiceBase::OnStart(DWORD dwArgc, LPTSTR *pszArgv)
{
}

//   FUNCTION: CServiceBase::OnStop()
//
//   PURPOSE: When implemented in a derived class, executes when a Stop 
//   command is sent to the service by the SCM. Specifies actions to take 
//   when a service stops running. Be sure to periodically call 
//   CServiceBase::SetServiceStatus() with SERVICE_STOP_PENDING if the 
//   procedure is going to take long time. 
//
void ServiceBase::OnStop()
{
}

//   FUNCTION: CServiceBase::OnPause()
//
//   PURPOSE: When implemented in a derived class, executes when a Pause 
//   command is sent to the service by the SCM. Specifies actions to take 
//   when a service pauses.
void ServiceBase::OnPause()
{
}

//   FUNCTION: CServiceBase::OnContinue()
//
//   PURPOSE: When implemented in a derived class, OnContinue runs when a 
//   Continue command is sent to the service by the SCM. Specifies actions to 
//   take when a service resumes normal functioning after being paused.
void ServiceBase::OnContinue()
{
}

//   FUNCTION: CServiceBase::OnShutdown()
//
//   PURPOSE: When implemented in a derived class, executes when the system 
//   is shutting down. Specifies what should occur immediately prior to the 
//   system shutting down.
void ServiceBase::OnShutdown()
{
}

//   FUNCTION: CServiceBase::SetServiceStatus(DWORD, DWORD, DWORD)
//
//   PURPOSE: The function sets the service status and reports the status to 
//   the SCM.
//
//   PARAMETERS:
//   * dwCurrentState - the state of the service
//   * dwWin32ExitCode - error code to report
//   * dwWaitHint - estimated time for pending operation, in milliseconds
void ServiceBase::SetServiceStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	//Fill in the SERVICE_STATUS structure of the service
	m_status.dwCurrentState = dwCurrentState;
	m_status.dwWin32ExitCode = dwWin32ExitCode;
	m_status.dwWaitHint = dwWaitHint;

	m_status.dwCheckPoint = (dwCurrentState == SERVICE_RUNNING) || 
							(dwCurrentState == SERVICE_STOPPED) ? 
							0 : dwCheckPoint++;
	//Report the status of the service to the SCM
	::SetServiceStatus(m_statusHandle, &m_status);
}
void ServiceBase::LogEvent(LPCTSTR pFormat, ...)
{
	TCHAR    chMsg[256];
	HANDLE  hEventSource;
	LPTSTR  lpszStrings[1];
	va_list pArg;

	va_start(pArg, pFormat);
	sprintf_s(chMsg, pFormat, pArg);
	va_end(pArg);

	lpszStrings[0] = chMsg;

	hEventSource = RegisterEventSource(NULL, TEXT(m_serviceName));
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource,				//Event log handle
			EVENTLOG_INFORMATION_TYPE,			//Event type
			0,									//Event category
			0,									//Event identifier
			NULL,								//No security identifier
			1,									//Size of lpszStrings array
			0,									//No binary data
			(LPCTSTR*)&lpszStrings[0],			//Array of strings
			NULL								//No binary data
		);
		DeregisterEventSource(hEventSource);
	}
}
#pragma endregion

#pragma region <pravite>
//   FUNCTION: CServiceBase::ServiceMain(DWORD, PWSTR *)
//
//   PURPOSE: Entry point for the service. It registers the handler function 
//   for the service and starts the service.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
void WINAPI ServiceBase::ServiceMain(DWORD dwArgc, LPTSTR *pszArgv)
{
	assert(m_service != NULL);

	//Register the handler function for the service
	m_service->m_statusHandle = RegisterServiceCtrlHandler(
							TEXT(m_service->GetServiceName()), ServiceCtrlHandler);
	if (m_service->m_statusHandle == NULL)
	{
		throw GetLastError();
	}

	//start the service
	m_service->Start(dwArgc, pszArgv);

}

//   FUNCTION: CServiceBase::ServiceCtrlHandler(DWORD)
//
//   PURPOSE: The function is called by the SCM whenever a control code is 
//   sent to the service. 
//
//   PARAMETERS:
//   * dwCtrlCode - the control code. This parameter can be one of the 
//   following values: 
//
//     SERVICE_CONTROL_CONTINUE
//     SERVICE_CONTROL_INTERROGATE
//     SERVICE_CONTROL_NETBINDADD
//     SERVICE_CONTROL_NETBINDDISABLE
//     SERVICE_CONTROL_NETBINDREMOVE
//     SERVICE_CONTROL_PARAMCHANGE
//     SERVICE_CONTROL_PAUSE
//     SERVICE_CONTROL_SHUTDOWN
//     SERVICE_CONTROL_STOP
//
//   This parameter can also be a user-defined control code ranges from 128 
//   to 255.
void WINAPI ServiceBase::ServiceCtrlHandler(DWORD dwCtrl)
{
	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		m_service->Stop();
		break;
	case SERVICE_CONTROL_PAUSE:
		m_service->Pause();
		break;
	case SERVICE_CONTROL_CONTINUE:
		m_service->Continue();
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		m_service->Shutdown();
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
}

//   FUNCTION: CServiceBase::Start(DWORD, PWSTR *)
//
//   PURPOSE: The function starts the service. It calls the OnStart virtual 
//   function in which you can specify the actions to take when the service 
//   starts. If an error occurs during the startup, the error will be logged 
//   in the Application event log, and the service will be stopped.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
void ServiceBase::Start(DWORD dwArgc, LPTSTR *pszArgv)
{
	try {
		LogEvent("service start pending.");
		//Tell SCM that the service is starting
		SetServiceStatus(SERVICE_START_PENDING);
		//Perform service-specific initialization
		OnStart(dwArgc, pszArgv);
		//Tell SCM that the service is started
		SetServiceStatus(SERVICE_RUNNING);
		LogEvent("service running.");
	}
	catch (...) {
		//Log the error
		LogEvent("service failed to start");
		//Set the service status to be stopped
		SetServiceStatus(SERVICE_STOPPED);
	}
}

//   FUNCTION: CServiceBase::Pause()
//
//   PURPOSE: The function pauses the service if the service supports pause 
//   and continue. It calls the OnPause virtual function in which you can 
//   specify the actions to take when the service pauses. If an error occurs, 
//   the error will be logged in the Application event log, and the service 
//   will become running.
void ServiceBase::Pause()
{
	try
	{
		LogEvent("service pause pending.");
		// Tell SCM that the service is pausing.
		SetServiceStatus(SERVICE_PAUSE_PENDING);
		// Perform service-specific pause operations.
		OnPause();
		// Tell SCM that the service is paused.
		SetServiceStatus(SERVICE_PAUSED);
		LogEvent("service paused.");
	}
	catch (...)
	{
		// Log the error.
		LogEvent("Service failed to pause.");

		// Tell SCM that the service is still running.
		SetServiceStatus(SERVICE_RUNNING);
	}
}

//   FUNCTION: CServiceBase::Continue()
//
//   PURPOSE: The function resumes normal functioning after being paused if
//   the service supports pause and continue. It calls the OnContinue virtual 
//   function in which you can specify the actions to take when the service 
//   continues. If an error occurs, the error will be logged in the 
//   Application event log, and the service will still be paused.
void ServiceBase::Continue()
{
	try
	{
		LogEvent("service continue pending.");
		//Tell SCM that the service is resuming
		SetServiceStatus(SERVICE_CONTINUE_PENDING);
		//Perform service-specific continue operation
		OnContinue();
		//Tell SCM that the service is running
		SetServiceStatus(SERVICE_RUNNING);
		LogEvent("service running.");
	}
	catch (...)
	{
		// Log the error.
		LogEvent("Service failed to resume.");
		// Tell SCM that the service is still paused.
		SetServiceStatus(SERVICE_PAUSED);
	}
}

//   FUNCTION: CServiceBase::Shutdown()
//
//   PURPOSE: The function executes when the system is shutting down. It 
//   calls the OnShutdown virtual function in which you can specify what 
//   should occur immediately prior to the system shutting down. If an error 
//   occurs, the error will be logged in the Application event log.
void ServiceBase::Shutdown()
{
	try
	{
		LogEvent("service shutdown pending.");
		// Perform service-specific shutdown operations.
		OnShutdown();
		// Tell SCM that the service is stopped.
		SetServiceStatus(SERVICE_STOPPED);
		LogEvent("service shutdown.");
	}
	catch (...)
	{
		// Log the error.
		LogEvent("Service failed to shutdown.");
	}
}
#pragma endregion