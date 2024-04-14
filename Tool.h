#ifndef TOOL_H
#define TOOL_H
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <stdio.h>
#include <string.h>
//#include <windows.h>
#define MAXSERVER 5

static int globalID = 0;
struct pingnode {
	char name[10];
	char ip[25];
	char label[20];
	char* result;//no free memory
	void* parent;
	int id;
	struct pingnode* next;
};
typedef struct pingnode strPingNode;
struct pingconfig {
	char server[30];
	char* url;//get data no free memory
	char refresfrate[3];
	char flushafter[6];
	char isconnected;
	//HANDLE mythread;
	strPingNode* arrayPingNode;//=head
};
typedef struct pingconfig strPingCfg;

struct sqlnode {
	char name[20];
	char key[20];
	char isUsed;
	char result[5];
	int id;
	void* parent;
	struct sqlnode* next;
};
typedef struct sqlnode strSQLnode;
struct sqlconfig {
	char server[30];
	char *query;//no free memory
	char* url;//get data no free memory
	char refresfrate[6];
	char flushafter[200];
	char label[10];
	char isconnected;
	//HANDLE mythread;
	strSQLnode* arraySqlNode;
};
typedef struct sqlconfig strSQLCfg;
enum type {
	sql,
	ping
};
typedef enum type eType;
struct threadparam {
	eType ptype;
	void* config;
};
char* concatenate_strings(eType type, void* cfg, int num_chars);
int parsingConfig(const char* file);
void printLinkedListPing(struct pingnode* head);
#endif