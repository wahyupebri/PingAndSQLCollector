#include "Tool.h"
//linkedlist
int globalPingServerNumber = 0;
int globalSQLServerNumber = 0;
strSQLCfg* SQLcfgArray[MAXSERVER];
strPingCfg* PingCfgArray[MAXSERVER];
// Function to create a new node with the given data
struct pingnode* createPingNode(char* name, char* ip, char* label) {
	struct pingnode* newNode = (struct pingnode*)malloc(sizeof(struct pingnode));
	if (newNode != NULL) {
		strcpy(newNode->ip, ip);
		strcpy(newNode->name, name);
		strcpy(newNode->label, label);
		newNode->id = globalID;
		newNode->result = NULL;
		globalID++;
		newNode->next = NULL;
	}
	return newNode;
}

// Function to insert a new node at the beginning of the linked list
struct pingnode* insertAtPingBeginning(struct pingnode* head, char* name, char* ip, char* label, strPingCfg* pPingcfg) {
	struct pingnode* newNode = createPingNode(name, ip, label);
	if (newNode != NULL) {
		newNode->next = head;
		head = newNode;
		newNode->parent = pPingcfg;
	}
	return head;
}

// Function to print the elements of the linked list
void printLinkedListPing(struct pingnode* head) {
	struct pingnode* current = head;
	while (current != NULL) {
		printf("Name %s ip %s label %s;", current->name, current->ip, current->label);
		current = current->next;
	}
	printf("NULL\n");
}

// Function to free the memory allocated for the linked list
void freeLinkedListPing(struct pingnode* head) {
	struct pingnode* current = head;
	while (current != NULL) {
		struct pingnode* temp = current;
		current = current->next;
		free(temp);
	}
}
//end--

void* parseXmlNodePingChild(xmlNodePtr node, strPingCfg* pPingcfg, struct pingnode* head) {
	xmlNodePtr curNode = NULL;
	curNode = node;


	if (curNode->type == XML_ELEMENT_NODE) {
		//printf("Child Child Node: %s\n", (char*)curNode->name);

		xmlNodePtr childNode = NULL;
		strPingNode temp;
		char tempName[10];
		for (childNode = curNode->children; childNode; childNode = childNode->next) {
			//Pnode temp;
			if (childNode->type == XML_ELEMENT_NODE) {
				if (!strcmp((char*)childNode->name, "name")) {
					//printf("Name %s \n", (char*)xmlNodeGetContent(childNode));
					strcpy(tempName, (char*)xmlNodeGetContent(childNode));

				}
				else if (!strcmp((char*)childNode->name, "details")) {
					//printf("Details %s \n", (char*)xmlNodeGetContent(childNode));
					xmlNodePtr dtlsNode = childNode->children;
					for (; dtlsNode; dtlsNode = dtlsNode->next) {
						if (dtlsNode->type == XML_ELEMENT_NODE) {
							//printf("Child Details %s \n", (char*)xmlNodeGetContent(dtlsNode));
							if (!strcmp((char*)dtlsNode->name, "label")) {
								strcpy(temp.label, (char*)xmlNodeGetContent(dtlsNode));

							}
							if (!strcmp((char*)dtlsNode->name, "ip")) {
								strcpy(temp.ip, (char*)xmlNodeGetContent(dtlsNode));

							}
						}
					}
					head = insertAtPingBeginning(head, tempName, temp.ip, temp.label, pPingcfg);
					//head->next = NULL;//ADD
				}

			}
			//head = insertAtPingBeginning(head, tempName, temp.ip, temp.label);
		}
		//head = insertAtPingBeginning(head, tempName, temp.ip, temp.label);
	}
	return head;
}
void printPingCfg(int maxcounter) {
	int c = 0;
	for (;c < maxcounter;c++) {
		printf("Print all %d %s %s %s\n", c, PingCfgArray[c]->flushafter, PingCfgArray[c]->refresfrate, PingCfgArray[c]->server);
		printLinkedListPing(PingCfgArray[c]->arrayPingNode);
	}
}
void parseXmlNodePing(xmlNodePtr node, int counter) {
	struct pingnode* head = NULL;
	xmlNodePtr curNode = NULL;
	curNode = node;
	strPingCfg* newPingcfg = (strPingCfg*)malloc(sizeof(strPingCfg));
	if (newPingcfg == NULL) {
		return;
	}
	PingCfgArray[counter] = newPingcfg;

	if (curNode->type == XML_ELEMENT_NODE) {
		//printf("Node: %s\n", (char*)curNode->name);

		xmlNodePtr childNode = NULL;
		for (childNode = curNode->children; childNode; childNode = childNode->next) {
			if (childNode->type == XML_ELEMENT_NODE) {
				//printf("\tChild Node: %s, Value: %s\n", childNode->name, xmlNodeGetContent(childNode));
				if (!strcmp((char*)childNode->name, "node")) {
					head = (struct pingnode*)parseXmlNodePingChild(childNode, newPingcfg, head);
				}
				else if (!strcmp((char*)childNode->name, "attempt")) {
					//refresh
					strcpy(newPingcfg->refresfrate, (char*)xmlNodeGetContent(childNode));
				}
				else if (!strcmp((char*)childNode->name, "ttl")) {
					//flushafter
					strcpy(newPingcfg->flushafter, (char*)xmlNodeGetContent(childNode));
				}
				else if (!strcmp((char*)childNode->name, "server")) {
					//server
					strcpy(newPingcfg->server, (char*)xmlNodeGetContent(childNode));
				}
			}
		}
		//parseXmlNodePing(childNode); // Recursively parse child nodes
	}
	//printf("Hasil %s %s %s\n", newPingcfg->flushafter, newPingcfg->refresfrate, newPingcfg->server);
	newPingcfg->arrayPingNode = head;
	int lenurl = strlen("http://");
	char* pserver = NULL;
		lenurl += strlen(newPingcfg->server);
		pserver = newPingcfg->server;
	lenurl += strlen("/getdata");
	char* link = (char*)malloc(sizeof(char) * (lenurl + 1));
	strcpy(link, "http://");
	strcat(link, pserver);
	strcat(link, "/getdata");
	*(link + lenurl) = '\0';
	newPingcfg->url = link;

	//head->next = NULL;//ADD!!
}
// Define the structure for a node in the linked list

// Function to create a new node with the given data
struct sqlnode* createSqlNode(char* name, char* key) {
	struct sqlnode* newNode = (struct sqlnode*)malloc(sizeof(struct sqlnode));
	if (newNode != NULL) {
		strcpy(newNode->key, key);
		strcpy(newNode->name, name);
		newNode->id = globalID;
		globalID++;
		newNode->next = NULL;
	}
	return newNode;
}

// Function to insert a new node at the beginning of the linked list
struct sqlnode* insertAtSqlBeginning(struct sqlnode* head, char* name, char* key, strSQLCfg* pSqlcfg) {
	struct sqlnode* newNode = createSqlNode(name, key);
	if (newNode != NULL) {
		newNode->next = head;
		head = newNode;
		strcpy(newNode->result,"-1");
	}
	newNode->parent = pSqlcfg;
	return head;
}

// Function to print the elements of the linked list
void printLinkedListSql(struct sqlnode* head) {
	struct sqlnode* current = head;
	while (current != NULL) {
		printf("Name %s key %s;", current->name, current->key);
		current = current->next;
	}
	printf("NULL\n");
}

// Function to free the memory allocated for the linked list
void freeLinkedListSql(struct sqlnode* head) {
	struct sqlnode* current = head;
	while (current != NULL) {
		struct sqlnode* temp = current;
		current = current->next;
		free(temp);
	}
}
//end--



void* parseXmlNodeSqlChild(xmlNodePtr node, strSQLCfg* pSqlcfg, struct sqlnode* head) {
	xmlNodePtr curNode = NULL;
	curNode = node;


	if (curNode->type == XML_ELEMENT_NODE) {
		//printf("Child Child Node: %s\n", (char*)curNode->name);

		xmlNodePtr childNode = NULL;
		strSQLnode temp;
		for (childNode = curNode->children; childNode; childNode = childNode->next) {
			//Pnode temp;
			if (childNode->type == XML_ELEMENT_NODE) {
				if (!strcmp((char*)childNode->name, "name")) {
					//printf("Name %s \n", (char*)xmlNodeGetContent(childNode));
					strcpy(temp.name, (char*)xmlNodeGetContent(childNode));

				}
				if (!strcmp((char*)childNode->name, "key")) {
					//printf("Name %s \n", (char*)xmlNodeGetContent(childNode));
					strcpy(temp.key, (char*)xmlNodeGetContent(childNode));

				}


			}
			//head = insertAtPingBeginning(head, tempName, temp.ip, temp.label);
		}
		head = insertAtSqlBeginning(head, temp.name, temp.key, pSqlcfg);
	}
	return head;
}
void parseXmlNodeSQL(xmlNodePtr node, int counter) {
	xmlNodePtr curNode = NULL;
	curNode = node;
	struct sqlnode* head = NULL;
	strSQLCfg* newSQLcfg = (strSQLCfg*)malloc(sizeof(strSQLCfg));
	if (newSQLcfg == NULL) {
		return;
	}
	SQLcfgArray[counter] = newSQLcfg;
	if (curNode->type == XML_ELEMENT_NODE) {
		//printf("SQL Node: %s\n", (char*)curNode->name);
		xmlNodePtr childNode = NULL;
		for (childNode = curNode->children; childNode; childNode = childNode->next) {
			if (childNode->type == XML_ELEMENT_NODE) {
				if (!strcmp((char*)childNode->name, "node")) {
					head = (struct sqlnode*)parseXmlNodeSqlChild(childNode, newSQLcfg, head);
				}
				else if (!strcmp((char*)childNode->name, "refresh")) {
					//refresh
					strcpy(newSQLcfg->refresfrate, (char*)xmlNodeGetContent(childNode));
				}
				else if (!strcmp((char*)childNode->name, "dsn")) {
					//dsn
					char* ptr = (char*)xmlNodeGetContent(childNode);
					char* ptr_= ptr;
					while (*ptr_ != '\0') {
						if (*ptr_ == ';') {
							*ptr_ = ',';
						}
						if (*ptr_ == '#') {
							*ptr_ = '_';
						}
						ptr_++;
					}
					strcpy(newSQLcfg->flushafter, ptr);
				}
				else if (!strcmp((char*)childNode->name, "server")) {
					//server
					strcpy(newSQLcfg->server, (char*)xmlNodeGetContent(childNode));
				}
				else if (!strcmp((char*)childNode->name, "label")) {
					//server
					strcpy(newSQLcfg->label, (char*)xmlNodeGetContent(childNode));
				}
				else if (!strcmp((char*)childNode->name, "query")) {
					//server
					printf("INFO: query %s. Len %d\n", (char*)xmlNodeGetContent(childNode), strlen((char*)xmlNodeGetContent(childNode)));
					int strlength = strlen((char*)xmlNodeGetContent(childNode));
					char* temp = (char*)malloc(sizeof(char) * (strlength + 1));
					strcpy(temp, (char*)xmlNodeGetContent(childNode));
					*(temp + strlength + 1) = '\0';
					char* before_temp = temp;
					while (*temp != '\0') {
						if (*temp == 32) {
							*temp = 43;
						}
						temp++;
					}
					newSQLcfg->query = before_temp;
				}
			}

		}
	}
	newSQLcfg->arraySqlNode = head;
	int lenurl = strlen("http://");
	char* pserver = NULL;
	lenurl += strlen(newSQLcfg->server);
	pserver = newSQLcfg->server;
	lenurl += strlen("/getdata");
	char* link = (char*)malloc(sizeof(char) * (lenurl + 1));
	strcpy(link, "http://");
	strcat(link, pserver);
	strcat(link, "/getdata");
	*(link + lenurl) = '\0';
	newSQLcfg->url = link;
}
//--
void printSQLcfg(int maxcounter) {
	int c = 0;
	for (;c < maxcounter;c++) {
		printf("Print all %d %s %s %s %s %s\n", c, SQLcfgArray[c]->flushafter, SQLcfgArray[c]->refresfrate, SQLcfgArray[c]->server, SQLcfgArray[c]->label, SQLcfgArray[c]->query);
		printLinkedListSql(SQLcfgArray[c]->arraySqlNode);
	}
}
int writeHTML() {
	FILE* file;
	char data[] = "This is some data that we will write to the file.";

	// Open the file in write mode
	file = fopen("example.txt", "w");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file for writing.\n");
		return 1;
	}

	// Write data to the file
	if (fwrite(data, sizeof(char), sizeof(data), file) != sizeof(data)) {
		fprintf(stderr, "Failed to write data to file.\n");
		fclose(file);
		return 1;
	}

	// Close the file
	fclose(file);

	printf("Data written to file successfully.\n");
}
void buildHTML(strPingCfg** ping, strSQLCfg** sql) {
	FILE* file;
	file = fopen("index.html", "w");
	char buffer[100];
	if (file == NULL) {
		fprintf(stderr, "ERROR: Failed to open file for writing.\n");
		return;
	}
	// Write data to the file
	fprintf(file, "<html><title>FMS Network Monitor</title><link rel=\"stylesheet\" type=\"text/css\" href =\"styles.css\"> <body><div id=\"title\">FMS Network Monitor");
	// Close the file
	//fclose(file);

	//printf("<html><body><div id=\"title\">All Monitoring Tool\n");
	for (int c = 0;c < globalPingServerNumber;c++) {
		struct pingnode* curPing = ping[c]->arrayPingNode;
		strSQLnode* head = NULL;
		strPingNode* prev = NULL;
		while (curPing != NULL) {
			if (prev == NULL) {
				fprintf(file, "</div>\n<div class=\"container\">\n");
				fprintf(file, "<div class=\"name\">%s</div>\n", curPing->name);
				prev = curPing;
			}
			if (curPing->next != NULL) {
				if (strcmp(curPing->name, curPing->next->name) != 0) {
					prev = NULL;
				}
			}

			fprintf(file, "<div class=\"node\" id=\"%d\">%s</div>\n", curPing->id, curPing->label);

			strPingNode* nextPing = curPing->next;
			if (nextPing != NULL) {
				if (strcmp(nextPing->name, curPing->name) != 0) {
					for (int cc = 0;cc < globalSQLServerNumber;cc++) {
						strSQLnode* curSQL = sql[cc]->arraySqlNode;
						while (curSQL != NULL) {
							if (strcmp(curPing->name, curSQL->name) == 0 && curSQL->isUsed != 1) {
								strSQLCfg* pSQLCfg = (strSQLCfg*)curSQL->parent;
								fprintf(file, "<div class=\"node\" id=\"%d\">%s</div>\n", curSQL->id, pSQLCfg->label);
								//printf("%d %s label %s %s\n",curSQL->id,curSQL->name, pSQLCfg->label, pSQLCfg->query);
								//head=insertAtSqlBeginning(head, curSQL->name, curSQL->key, pSQLCfg);
								curSQL->isUsed = 1;
							}
							curSQL = curSQL->next;
						}
					}
				}
			}

			else {
				for (int cc = 0;cc < globalSQLServerNumber;cc++) {
					strSQLnode* curSQL = sql[cc]->arraySqlNode;
					while (curSQL != NULL) {
						if (strcmp(curPing->name, curSQL->name) == 0 && curSQL->isUsed != 1) {
							strSQLCfg* pSQLCfg = (strSQLCfg*)curSQL->parent;
							fprintf(file, "<div class=\"node\" id=\"%d\">%s</div>\n", curSQL->id, pSQLCfg->label);
							//printf("%d %s label %s %s\n", curSQL->id, curSQL->name, pSQLCfg->label, pSQLCfg->query);
							//head=insertAtSqlBeginning(head, curSQL->name, curSQL->key, pSQLCfg);
							curSQL->isUsed = 1;
						}
						curSQL = curSQL->next;
					}
				}
			}
			curPing = curPing->next;
		}
		//printLinkedListSql(head);
		//printf("NULL\n");
	}
	fprintf(file, "</div></body>\n<script src=\"https://code.jquery.com/jquery-3.6.0.min.js\"></script>\n<script src=\"wahyu.js\"></script>\n</html>\n");
	fclose(file);
}
void parseXmlNode(xmlNodePtr node) {
	xmlNodePtr curNode = NULL;
	int counterPingNode = 0;
	int counterSqlNode = 0;
	for (curNode = node; curNode; curNode = curNode->next) {
		if (curNode->type == XML_ELEMENT_NODE) {
			//printf("Node Parent: %s %d\n", (char *)curNode->name, counterPingNode);
			if (!strcmp((char*)curNode->name, "ping")) {
				if (counterPingNode < MAXSERVER) {
					parseXmlNodePing(curNode, counterPingNode);
					counterPingNode++;
				}
				else {
					fprintf(stderr, "ERROR: Maximum Ping Server\n");
				}

			}
			if (!strcmp((char*)curNode->name, "sql")) {
				if (counterSqlNode < MAXSERVER) {
					parseXmlNodeSQL(curNode, counterSqlNode);
					counterSqlNode++;
				}
				else {
					fprintf(stderr, "ERROR: Maximum SQL Server\n");
				}
			}
			/*xmlNodePtr childNode = NULL;
			for (childNode = curNode->children; childNode; childNode = childNode->next) {
				if (childNode->type == XML_ELEMENT_NODE) {
					printf("\tChild Node: %s, Value: %s\n", childNode->name, xmlNodeGetContent(childNode));
				}
			}*/

			//parseXmlNode(curNode->children); // Recursively parse child nodes
		}
	}
	globalPingServerNumber = counterPingNode;
	globalSQLServerNumber = counterSqlNode;
	//printPingCfg(globalPingServerNumber);
	//printSQLcfg(globalSQLServerNumber);
	buildHTML(PingCfgArray, SQLcfgArray);
}
static void readConfig(const char* filename) {
	xmlDocPtr doc;
	xmlNodePtr rootNode;

	LIBXML_TEST_VERSION;

	doc = xmlReadFile(filename, NULL, 0);
	if (doc == NULL) {
		fprintf(stderr, "Failed to parse %s \n", filename);
		return;
	}
	rootNode = xmlDocGetRootElement(doc);
	parseXmlNode(rootNode->children);
	xmlFreeDoc(doc);
}
int parsingConfig(const char* file) {
	readConfig(file);

	xmlCleanupParser();
	xmlMemoryDump();
	return 0;
}
char* concatenate_strings(eType type, void* cfg, int num_chars) {
	// Allocate memory for the concatenated string
	char* result = (char*)malloc(num_chars + 1); // +1 for null terminator
	if (result == NULL) {
		perror("ERROR: Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	// Copy each string into the concatenated string
	char* ptr = result;
	if (type == type::sql) {

		strSQLCfg* pcfg = (strSQLCfg*)cfg;
		strcpy(ptr, "http://");
		ptr += strlen("http://");
		strcpy(ptr, pcfg->server);
		ptr += strlen(pcfg->server);
		strcpy(ptr, "/setup?config=");
		ptr += strlen("/setup?config=");
		strcpy(ptr, "sql;");
		ptr += strlen("sql;");

		strcpy(ptr, pcfg->flushafter);
		ptr += strlen(pcfg->flushafter);
		strcpy(ptr, ";");
		ptr += 1;
		strcpy(ptr, pcfg->refresfrate);
		ptr += strlen(pcfg->refresfrate);
		strcpy(ptr, ";");
		ptr += 1;
		strcpy(ptr, pcfg->query);
		ptr += strlen(pcfg->query);
		//strcpy(ptr, ",");
		//ptr += 1;
		strSQLnode* head = pcfg->arraySqlNode;
		while (head != NULL) {
			strcpy(ptr, ";");
			ptr += 1;
			strcpy(ptr, head->key);
			ptr += strlen(head->key);

			head = head->next;
		}
		strcpy(ptr, ";end");
		ptr += strlen(";end");
	}
	else if (type == type::ping) {
		//char* ptr = result;
		strPingCfg* pcfg = (strPingCfg*)cfg;
		strcpy(ptr, "http://");
		ptr += strlen("http://");
		strcpy(ptr, pcfg->server);
		ptr += strlen(pcfg->server);
		strcpy(ptr, "/setup?config=");
		ptr += strlen("/setup?config=");
		strcpy(ptr, "ping,");
		ptr += strlen("ping,");
		strcpy(ptr, pcfg->flushafter);
		ptr += strlen(pcfg->flushafter);
		strcpy(ptr, ",");
		ptr += 1;
		strcpy(ptr, pcfg->refresfrate);
		ptr += strlen(pcfg->refresfrate);

		strPingNode* head = pcfg->arrayPingNode;
		while (head != NULL) {
			strcpy(ptr, ",");
			ptr += 1;
			strcpy(ptr, head->ip);
			ptr += strlen(head->ip);

			head = head->next;
		}
		strcpy(ptr, ",end");
		ptr += strlen(",end");
	}
	// Null terminate the concatenated string
	*ptr = '\0';

	return result;
}