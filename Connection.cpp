#include "Connection.h"
#include "curl/curl.h"
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo</body></html>"
extern int globalPingServerNumber;
extern int globalSQLServerNumber;
extern strSQLCfg* SQLcfgArray[MAXSERVER];
extern strPingCfg* PingCfgArray[MAXSERVER];
cJSON* dataArray = NULL;
extern HANDLE hMutex;
const char* RESULTNOTCONNECTED = "-2";
char sharedSignal=1;
// Function to insert data into the JSON array
void insertData(int id, const char* value) {
    // Create cJSON object for the new data
    cJSON* newData = cJSON_CreateObject();
    cJSON_AddNumberToObject(newData, "id", id);
    cJSON_AddStringToObject(newData, "value", value);

    // Add the new data object to the array
    if (dataArray == NULL) {
        dataArray = cJSON_CreateArray();
    }
    cJSON_AddItemToArray(dataArray, newData);
}

// Function to produce JSON string
char* produceJSON() {
    if (dataArray == NULL) {
        return NULL; // No data, return NULL
    }

    // Convert cJSON array to JSON string
    char* jsonString = cJSON_Print(dataArray);

    return jsonString;
}
char* createMyJSON(strPingCfg** pingcfgarr) {
    //count length
    int pingresultlen = 0;
    int sqlresultlen = 0;
    char* pingjson = NULL;
    strPingCfg* pPingCfg = *pingcfgarr;
    char* jsonString = NULL;
    //WaitForSingleObject(hMutex, INFINITE);
    if (dataArray == NULL) {
        //printf("%p %p\n", pPingCfg, pingcfgarr[0]);
        for (int p = 0;p < globalPingServerNumber;) {
            strPingNode* head = pingcfgarr[p]->arrayPingNode;
            //printLinkedListPing(head);
            while (head != NULL) {
                if (pingcfgarr[p]->isconnected == 1) {
                    insertData(head->id, head->result);
                    //free(head->result); //NEED CHECK!!
                    //printf("%p %s\n", head, head->ip);
                }
                else
                    insertData(head->id, RESULTNOTCONNECTED);
                    
                head = head->next;
            }
            p++;
        }
    }
    //ReleaseMutex(hMutex);
        // Produce JSON string
    jsonString = produceJSON();
    if (jsonString != NULL) {
        *(jsonString + strlen(jsonString)) = '\0';
        //printf("--JSON: %s\n", jsonString);
    }
    else {
        printf("WARNING: No data to produce JSON.\n");
    }


    //free(dataArray);

     //Clean up cJSON array
    if (dataArray != NULL) {
        cJSON_Delete(dataArray);
        dataArray = NULL;
    }

    /*for (int p = 0;p < globalSQLServerNumber;p++) {
        strSQLnode* head = sqlcfgarr[p].arraySqlNode;
        while (head != NULL) {
            sqlresultlen+=strlen(head->result);
        }
    }*/

    return jsonString;
}
int parsingPingResult(char* buffer, strPingCfg* ppingcfg) {
    int pcounter = 0;
    int lastIPcounter = 0;
    strPingNode* head = ppingcfg->arrayPingNode;
    //char * newbuffer=(char *)malloc(sizeof(char)*strlen(buffer))
    if (buffer != NULL) {
        //WaitForSingleObject(hMutex, INFINITE);
        while (true) {
            if (*buffer == ';' || *buffer == '\0') {
                char* temp = (char*)malloc((pcounter - lastIPcounter + 1) * sizeof(char));
                char *temp_url = buffer - (pcounter - lastIPcounter);
                strncpy(temp, temp_url, (pcounter - lastIPcounter + 1));
                *(temp+ (pcounter - lastIPcounter)) = '\0';
                if (head->result != NULL && strcmp(head->result,RESULTNOTCONNECTED) != 0) {
                    free(head->result);
                    head->result = NULL;
                }
                    
                head->result = temp;
                lastIPcounter = pcounter + 1;
                head = head->next;
                if (*buffer == '\0') {
                    break;
                }
            }
            buffer++;
            pcounter++;

        }

    }
    else {
        printf("WARNING: Buffer NULL parsing Result %s id %d\n", head->ip, head->id);
    }

    return 0;
}
size_t curlCallbackResult(void* ptr, size_t size, size_t nmemb, char** userdata) {
    size_t total = size * nmemb;
    *userdata = (char*)realloc(*userdata, total + 1);
    memcpy(*userdata, ptr, total);
    (*userdata)[total] = '\0';
    return total;
}
static int curlGetResult(eType type, void* config) {
    CURL* curl = curl_easy_init();
    CURLcode res;
    char* buffCurl = NULL;
 
    char* link=NULL;
    if (type == eType::sql) {
        strSQLCfg* pcfg = (strSQLCfg*)config;
        link=pcfg->url;
    }
    if (type == eType::ping) {
        strPingCfg* pcfg = (strPingCfg*)config;
        link = pcfg->url;
    }
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, link);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallbackResult);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffCurl);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "ERROR: failed to connect URL %s, %s\n", link, curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            buffCurl = NULL;
            return -1; //**WAJIB
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    else {
        return -1;
    }
    if (buffCurl != NULL && res == CURLE_OK) {
        printf("INFO: get result from %s\n", link);
        if (type == eType::ping) {
            parsingPingResult(buffCurl, (strPingCfg*)config);
            free(buffCurl);
            buffCurl = NULL;
            return 0;
        }
        /* else
             parsingPingResult(buffCurl, (strPingCfg*)config)*/
             //free(buffCurl);
                 /*return 0;
             }*/
    }
    else {
        printf("ERROR: No result from %s\n", link);
    }
    free(buffCurl);
    return 1;
}
size_t curlCallbackConfig(void* ptr, size_t size, size_t nmemb, char** userdata) {
    size_t total = size * nmemb;
    *userdata = (char*)realloc(*userdata, total + 1);
    memcpy(*userdata, ptr, total);
    (*userdata)[total] = '\0';
    return total;
}
int curlSendConfig(char* link) {
    CURL* curl = curl_easy_init();
    CURLcode res;
    char* buffCurl = NULL;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, link);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallbackConfig);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffCurl);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "ERROR: failed to connect server, %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return -1; //**WAJIB
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    else {
        return -1;
    }
    if (buffCurl) {
        if (strcmp(buffCurl, "1") == 0) {
            free(buffCurl);
            return 0;
        }
    }
    free(buffCurl);
    return 1;
}


int sendConfig(eType type, void* config) {
    int strLength = strlen("http://");
    if (type == sql) {
        strSQLCfg* pcfg = (strSQLCfg*)config;
        strLength += strlen(pcfg->server);
        strLength += strlen("/setup?config=");
        strLength += strlen("sql,");
        strLength += strlen(pcfg->flushafter) + 1;
        strLength += strlen(pcfg->refresfrate) + 1;
        strLength += strlen(pcfg->query) + 1;
        strSQLnode* head = pcfg->arraySqlNode;
        while (head != NULL) {
            strLength += strlen(head->key) + 1;
            head = head->next;
        }
        strLength += strlen("end");

    }
    else if (type == ping) {
        strPingCfg* pcfg = (strPingCfg*)config;
        strLength += strlen(pcfg->server);
        strLength += strlen("/setup?config=");
        strLength += strlen("ping,");
        strLength += strlen(pcfg->flushafter) + 1;
        strLength += strlen(pcfg->refresfrate) + 1;
        strPingNode* head = pcfg->arrayPingNode;
        while (head != NULL) {
            strLength += strlen(head->ip) + 1;
            head = head->next;
        }
        strLength += strlen("end");

    }
    char* result = concatenate_strings(type, config, strLength);
    printf("INFO: link %s\n", result);
    if (curlSendConfig(result) == 0)
    {
        free(result);
        return 0;
    }
    free(result);
    return 1;
}
int threadConnection(eType type, void* config) {
    int result=sendConfig(type, config);

    //result = 1;//temporary
    /*if (result == 0) {
        while (true) {*/
            //get data from server
            
            
            /*if (type == eType::ping) {
                strPingCfg* ppingcfg = (strPingCfg*)config;
                ppingcfg->arrayPingNode*/
    Sleep(5000);
    if (result == 0) {
        if (type==eType::ping) {
            strPingCfg* ppingcfg = (strPingCfg*)config;
            ppingcfg->isconnected = 1;
        }
        else if (type == eType::sql) {
            strPingCfg* ppingcfg = (strPingCfg*)config;
            ppingcfg->isconnected = 1;
        }
        //curlGetResult(type, config);
    }
    else{
        if (type == eType::ping) {
            strPingCfg* ppingcfg = (strPingCfg*)config;
            ppingcfg->isconnected = 0;
        }
        else if (type == eType::sql) {
            strPingCfg* ppingcfg = (strPingCfg*)config;
            ppingcfg->isconnected = 0;
        }
    }
                /*const char* dummy = "1;2;3";
                parsingPingResult(dummy,(strPingCfg*)config);*/

            //}
            //reverse the result
            //lock and pass to strCfg::result 
            //every time there request to web server, lock and generate json to display
     /*   }
    }*/
    return 0;
}
DWORD WINAPI threadConnection(LPVOID lpParam) {

    struct threadparam* params = (struct threadparam*)lpParam;

    int result = sendConfig(params->ptype, params->config);
    Sleep(5000);
    int getresult = 0;
    if (result == 0) {
        if (params->ptype == eType::ping) {
            strPingCfg* ppingcfg = (strPingCfg*)params->config;
            ppingcfg->isconnected = 1;
        }
        else if (params->ptype == eType::sql) {
            strSQLCfg* psqlcfg = (strSQLCfg*)params->config;
            psqlcfg->isconnected = 1;
        }
        while (true) {
            WaitForSingleObject(hMutex, INFINITE);
            if (sharedSignal == 1) {
                getresult = curlGetResult(params->ptype, params->config);
                ReleaseMutex(hMutex);
                if (getresult < 0) {
                    if (params->ptype == eType::ping) {
                        
                        strPingCfg* ppingcfg = (strPingCfg*)params->config;
                        for (int p = 0;p < globalPingServerNumber;p++) {
                            strPingNode* head = ppingcfg->arrayPingNode;
                            while (head != NULL) {
                                head->result = (char*)RESULTNOTCONNECTED;
                                head = head->next;
                            }
                        }
                        
                    }
                    else if (params->ptype == eType::sql) {
                        //FILL here for SQL!!
                    }
                    
                }
                Sleep(1000);
            }
        }
    }
    else {
        if (params->ptype == eType::ping) {
            strPingCfg* ppingcfg = (strPingCfg*)params->config;
            ppingcfg->isconnected = 0;
        }
        else if (params->ptype == eType::sql) {
            strSQLCfg* psqlcfg = (strSQLCfg*)params->config;
            psqlcfg->isconnected = 0;
        }
    }

    return 0;
}


static enum MHD_Result ahc_echo(void* cls,
    struct MHD_Connection* connection,
    const char* url,
    const char* method,
    const char* version,
    const char* upload_data,
    size_t* upload_data_size,
    void** ptr) {
    static int dummy;
    const char* page = (const char*)cls;
    struct MHD_Response* response;
    int ret;

    if (0 != strcmp(method, "GET"))
        return MHD_NO; /* unexpected method */
    if (&dummy != *ptr)
    {
        /* The first time only the headers are valid,
           do not respond in the first round... */
        *ptr = &dummy;
        return MHD_YES;
    }
    if (0 != *upload_data_size)
        return MHD_NO; /* upload data in a GET!? */
    *ptr = NULL; /* clear context pointer */
    
    char* myjson = NULL;
    
    if (strcmp(url, "/update") == 0) {
        WaitForSingleObject(hMutex, INFINITE);
        sharedSignal = 0;
        while (sharedSignal) {
            Sleep(10);
        }
        if (sharedSignal == 0) {
            myjson = createMyJSON(PingCfgArray);
            sharedSignal = 1;
        }
        ReleaseMutex(hMutex);

        response = MHD_create_response_from_buffer(strlen(myjson),
            (void*)myjson,
            MHD_RESPMEM_MUST_COPY);
        MHD_add_response_header(response, "Content-Type", "application/json");
        MHD_add_response_header(response, "Access-Control-Allow-Origin", "*"); // Add this line
        MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, OPTIONS"); // Add this line
        MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type"); // Add this line

        ret = MHD_queue_response(connection,
            MHD_HTTP_OK,
            response);
        MHD_destroy_response(response);
        if (myjson != NULL)
            cJSON_free(myjson);
        return (enum MHD_Result)ret;
    }
    response = MHD_create_response_from_buffer(strlen(page),
        (void*)page,
        MHD_RESPMEM_MUST_COPY);

    ret = MHD_queue_response(connection,
        MHD_HTTP_OK,
        response);
    MHD_destroy_response(response);
    return (enum MHD_Result)ret;
}
int runWebServer(int port) {
    WaitForSingleObject(hMutex, INFINITE);
    sharedSignal = 1;
    ReleaseMutex(hMutex);
    struct MHD_Daemon* d;
    d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
        port,
        NULL,
        NULL,
        &ahc_echo,
        (char*)PAGE,
        MHD_OPTION_END);
    printf("INFO: webserver is running\n");
    if (d == NULL)
        return 1;
    (void)getc(stdin);
    MHD_stop_daemon(d);
    return 0;
}