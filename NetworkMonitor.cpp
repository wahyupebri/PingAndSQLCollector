// NetworkMonitor.cpp : Defines the entry point for the application.
//

#include "Tool.h"
#include "NetworkMonitor.h"

#include "Connection.h"

using namespace std;
extern int globalPingServerNumber;
extern int globalSQLServerNumber;
extern strSQLCfg* SQLcfgArray[MAXSERVER];
extern strPingCfg* PingCfgArray[MAXSERVER];
HANDLE hMutex;
DWORD WINAPI mythreadFunction(LPVOID lpParam) {
	// This function will be executed by the thread
	printf("Thread running...\n");
	return 0;
}
int main()
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL) {
		fprintf(stderr, "Failed to create mutex. Error code: %lu\n", GetLastError());
		return 1;
	}

	parsingConfig("config.xml");
	for (int p = 0;p < globalPingServerNumber;p++) {
		struct threadparam* tparams=(struct threadparam*)malloc(sizeof(struct threadparam));
		tparams->config = PingCfgArray[p];
		tparams->ptype = eType::ping;
		//printf("%p %p\n", *PingCfgArray, tparams->config);
		CreateThread(NULL, 0, threadConnection, (LPVOID)tparams, 0, NULL);
	}
	//printf("json %p\n", *PingCfgArray);
	//createMyJSON(PingCfgArray);
	//disable sql
	for (int p = 0;p < globalSQLServerNumber;p++) {
		struct threadparam* tparams = (struct threadparam*)malloc(sizeof(struct threadparam));
		tparams->config = SQLcfgArray[p];
		tparams->ptype = eType::sql;
		//threadConnection(sql, SQLcfgArray[p]);
		CreateThread(NULL, 0, threadConnection, (LPVOID)tparams, 0, NULL);
	}
	runWebServer(8080);
	
	return 0;
}
