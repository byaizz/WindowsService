#ifndef __SERVICEBASE_H__
#define __SERVICEBASE_H__

#include <Windows.h>

#define SERVICE_NAME_LENGTH 32

class ServiceBase {
public:

	// Service object constructor. The optional parameters (fCanStop, 
	// fCanShutdown and fCanPauseContinue) allow you to specify whether the 
	// service can be stopped, paused and continued, or be notified when 
	// system shutdown occurs.
	ServiceBase(const char *pszServiceName, const char *pszDisplayName);

	// Service object destructor. 
	virtual ~ServiceBase();

	char* GetDisplayName() { return m_displayName; };
	char* GetServiceName() { return m_serviceName; };
	bool InstallService();	//install
	bool UninstallService();//uninstall
	bool StartInstalledService();//start service

	//Run the service
	bool Run();

	//Stop the service
	void Stop();

protected:

	// When implemented in a derived class, executes when a Start command is 
	// sent to the service by the SCM or when the operating system starts 
	// (for a service that starts automatically). Specifies actions to take 
	// when the service starts.
	virtual void OnStart(DWORD dwArgc, LPTSTR *pszArgv);

	// When implemented in a derived class, executes when a Stop command is 
	// sent to the service by the SCM. Specifies actions to take when a 
	// service stops running.
	virtual void OnStop();

	// When implemented in a derived class, executes when a Pause command is 
	// sent to the service by the SCM. Specifies actions to take when a 
	// service pauses.
	virtual void OnPause();

	// When implemented in a derived class, OnContinue runs when a Continue 
	// command is sent to the service by the SCM. Specifies actions to take 
	// when a service resumes normal functioning after being paused.
	virtual void OnContinue();

	// When implemented in a derived class, executes when the system is 
	// shutting down. Specifies what should occur immediately prior to the 
	// system shutting down.
	virtual void OnShutdown();
	
	//check the service is installed or not
	bool IsInstalled();

	//log
	void LogEvent(LPCTSTR pFormat, ...);

	// Set the service status and report the status to the SCM.
	void SetServiceStatus(DWORD dwCurrentState,
		DWORD dwWin32ExitCode = NO_ERROR,
		DWORD dwWaitHint = 0);
	
private:
	// Entry point for the service. It registers the handler function for the 
	// service and starts the service.
	static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);

	// The function is called by the SCM whenever a control code is sent to 
	// the service.
	static void WINAPI ServiceCtrlHandler(DWORD dwCtrl);

	// Register the executable for a service with the Service Control Manager 
	// (SCM). After you call Run(ServiceBase), the SCM issues a Start command, 
	// which results in a call to the OnStart method in the service. This 
	// method blocks until the service has stopped.
	bool RunService(ServiceBase *service);

	// Start the service
	void Start(DWORD dwArgc, LPTSTR *pszArgv);

	//Pause the service
	void Pause();

	//Resume the service after being paused
	void Continue();

	//Execute when the system is shutting down
	void Shutdown();

private:
	//The singleton service instance
	static ServiceBase *m_service;

	//The name of the service
	char m_serviceName[SERVICE_NAME_LENGTH];
	char m_displayName[SERVICE_NAME_LENGTH];

	//The status of the service
	SERVICE_STATUS m_status;

	//The service status handle
	SERVICE_STATUS_HANDLE m_statusHandle;

};

#endif