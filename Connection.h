#ifndef CONNECTION_H
#define CONNECTION_H
#include <microhttpd.h>
#include <stdlib.h>
#include "Tool.h"

char* createMyJSON(strPingCfg** pingcfgarr);
int sendConfig(eType type, void* config);
DWORD WINAPI threadConnection(LPVOID lpParam);
int threadConnection(eType type, void* config);
int requestData(eType type, char* server, void* result);
int runWebServer(int port);
#endif