#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <time.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <ifaddrs.h>
#include <netinet/in.h>

#include "comm_def.h"

int pd_check_my_ip_address();
void pd_curl_send_alarm(int alarm_code);
void pd_get_curl_data(int alarm_code, char *curl_data);
void pd_send_sensor_data(int co, int co2, int temp, int voc);

struct MemoryStruct {
    char *memory;
    size_t size;
};

extern char MY_IP_ADDR[16];

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int pd_check_my_ip_address()
{
    struct ifaddrs *ifaddr, *ifa;
    char ip_address[INET_ADDRSTRLEN];
    int check_cnt = 0;

    memset(MY_IP_ADDR, 0x00, sizeof(MY_IP_ADDR));

    if (getifaddrs(&ifaddr) == -1) 
    {
        printf("getifaddrs fail\n");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_name && strcmp(ifa->ifa_name, "eth0") == 0) 
        {
            if (ifa->ifa_addr->sa_family == AF_INET) 
            {
                if (inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ip_address, sizeof(ip_address)) != NULL) 
                {
                    sprintf(MY_IP_ADDR, "%s", ip_address);
                    check_cnt++;
                    break;
                }
            }
        }
    }

    freeifaddrs(ifaddr);

    if (check_cnt <= 0)
        return -1;

    return 1;
    
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) 
{
    size_t realsize = size * nmemb;

    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if (userp->memory == NULL) 
    {
#ifdef _DEBUG_
        printf("[CURL] memory NULL\n");
#endif
        return 0; 
    }

    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0;  // 문자열 종료

    return realsize;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void pd_get_curl_data(int alarm_code, char *curl_data)
{
    xmlDocPtr doc;
    xmlNodePtr root, node;
    char *speaker_ip, *speaker_port, *server_ip, *server_port, *path;
    char *rackid, *shutter, *dspa1, *dspa2, *dspa3, *fireing, *fireend, *warning;

#ifdef _DEBUG_
    printf("[CURL] START pd_get_curl_data() !!!!!\n");
#endif


    doc = xmlParseFile("/opt/dspa/CurlConfig.xml");
    if (doc == NULL)
    {
#ifdef _DEBUG_
        printf("[CURL] Error: could not parse file\n");
#endif
        return;
    }

    root = xmlDocGetRootElement(doc);
    if (root == NULL)
    {
#ifdef _DEBUG_
        printf("[CURL] Error: empty document\n");
#endif
        xmlFreeDoc(doc);
        return;
    }

    node = root->children;
    while (node != NULL)
    {
        if (xmlStrcmp(node->name, (const xmlChar *)"Item") == 0)
        {
            xmlNodePtr speakerNode, speakerportNode, serverNode, serverportNode, pathNode; 
            xmlNodePtr rackidNode, shutterNode, dspa1Node, dspa2Node, dspa3Node, fireingNode;
            xmlNodePtr fireendNode, warningNode;

            // Network Speaker IP Address
            speakerNode = xmlFirstElementChild(node);
            if (speakerNode != NULL && xmlStrcmp(speakerNode->name, (const xmlChar *)"Speaker") == 0)
            {
                speaker_ip = (char *)xmlNodeGetContent(speakerNode);
            }
            else
            {
                speaker_ip = "Unknown";
            }

            // Speaker PORT No
            speakerportNode = xmlNextElementSibling(speakerNode);
            if (speakerportNode != NULL && xmlStrcmp(speakerportNode->name, (const xmlChar *)"SpeakerPort") == 0)
            {
                speaker_port = (char *)xmlNodeGetContent(speakerportNode);
            }
            else
            {
                speaker_port = "Unknown";
            }

            // Server IP Address
            serverNode = xmlNextElementSibling(speakerportNode);
            if (serverNode != NULL && xmlStrcmp(serverNode->name, (const xmlChar *)"Server") == 0)
            {
                server_ip = (char *)xmlNodeGetContent(serverNode);
            }
            else
            {
                server_ip = "Unknown";
            }

            // Server PORT No
            serverportNode = xmlNextElementSibling(serverNode);
            if (serverportNode != NULL && xmlStrcmp(serverportNode->name, (const xmlChar *)"ServerPort") == 0)
            {
                server_port = (char *)xmlNodeGetContent(serverportNode);
            }
            else
            {
                server_port = "Unknown";
            }

            // path 
            pathNode = xmlNextElementSibling(serverportNode);
            if (pathNode != NULL && xmlStrcmp(pathNode->name, (const xmlChar *)"Path") == 0)
            {
                path = (char *)xmlNodeGetContent(pathNode);
            }
            else
            {
                path = "Unknown";
            }

            // rackid
            rackidNode = xmlNextElementSibling(pathNode);
            if (rackidNode != NULL && xmlStrcmp(rackidNode->name, (const xmlChar *)"RackId") == 0)
            {
                rackid = (char *)xmlNodeGetContent(rackidNode);
            }
            else
            {
                rackid = "Unknown";
            }

            // shutter
            shutterNode = xmlNextElementSibling(rackidNode);
            if (shutterNode != NULL && xmlStrcmp(shutterNode->name, (const xmlChar *)"Shutter") == 0)
            {
                shutter = (char *)xmlNodeGetContent(shutterNode);
            }
            else
            {
                shutter = "Unknown";
            }

            // dspa1
            dspa1Node = xmlNextElementSibling(shutterNode);
            if (dspa1Node != NULL && xmlStrcmp(dspa1Node->name, (const xmlChar *)"Dspa1") == 0)
            {
                dspa1 = (char *)xmlNodeGetContent(dspa1Node);
            }
            else
            {
                dspa1 = "Unknown";
            }

            // dspa2
            dspa2Node = xmlNextElementSibling(dspa1Node);
            if (dspa2Node != NULL && xmlStrcmp(dspa2Node->name, (const xmlChar *)"Dspa2") == 0)
            {
                dspa2 = (char *)xmlNodeGetContent(dspa2Node);
            }
            else
            {
                dspa2 = "Unknown";
            }

            // dspa3
            dspa3Node = xmlNextElementSibling(dspa1Node);
            if (dspa3Node != NULL && xmlStrcmp(dspa3Node->name, (const xmlChar *)"Dspa3") == 0)
            {
                dspa3 = (char *)xmlNodeGetContent(dspa3Node);
            }
            else
            {
                dspa3 = "Unknown";
            }

            // fireing 
            fireingNode = xmlNextElementSibling(dspa3Node);
            if (fireingNode != NULL && xmlStrcmp(fireingNode->name, (const xmlChar *)"FireIng") == 0)
            {
                fireing = (char *)xmlNodeGetContent(fireingNode);
            }
            else
            {
                fireing = "Unknown";
            }

            // fireend
            fireendNode = xmlNextElementSibling(fireingNode);
            if (fireendNode != NULL && xmlStrcmp(fireendNode->name, (const xmlChar *)"FireEnd") == 0)
            {
                fireend = (char *)xmlNodeGetContent(fireendNode);
            }
            else
            {
                fireend = "Unknown";
            }

            // warning
            warningNode = xmlNextElementSibling(fireendNode);
            if (warningNode != NULL && xmlStrcmp(warningNode->name, (const xmlChar *)"Warning") == 0)
            {
                warning = (char *)xmlNodeGetContent(warningNode);
            }
            else
            {
                warning = "Unknown";
            }

#ifdef _DEBUG_
            //printf("[CURL] Speaker: %s, Server: %s, path: %s, rackid: %s, shutter: %s, dspa1: %s, dspa2: %s, dspa3: %s, fireing: %s, fireend; %s\n", speaker_ip, server_ip, path, rackid, shutter, dspa1, dspa2, dspa3, fireing, fireend);
#endif

            if (alarm_code == ANN_FIRE_START)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, rackid);
            else if (alarm_code == ANN_SHUTTER)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, shutter);
            else if (alarm_code == ANN_DSPA1)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, dspa1);
            else if (alarm_code == ANN_DSPA2)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, dspa2);
            else if (alarm_code == ANN_DSPA3)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, dspa3);
            else if (alarm_code == ANN_FIRE_ING)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, fireing);
            else if (alarm_code == ANN_FIRE_END)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, fireend);
            else if (alarm_code == ANN_WARNING)
                sprintf(curl_data, "http://%s:%s/%s_%s", speaker_ip, speaker_port, path, warning);
            else
                sprintf(curl_data, "http://%s:%s", server_ip, server_port);

            xmlFree(speaker_ip);
            xmlFree(speaker_port);
            xmlFree(server_ip);
            xmlFree(server_port);
            xmlFree(path);
            xmlFree(rackid);
            xmlFree(shutter);
            xmlFree(dspa1);
            xmlFree(dspa2);
            xmlFree(dspa3);
            xmlFree(fireing);
            xmlFree(fireend);
            xmlFree(warning);
        }
        node = node->next;
    }

    // XML 문서 해제
    xmlFreeDoc(doc);
    return ;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void pd_curl_send_alarm(int alarm_code)
{
    CURL     *curl;
    CURLcode res;
    char curl_data[256];

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  
    chunk.size = 0;  

    memset(&curl_data, 0x00, sizeof(curl_data));

    (void) pd_get_curl_data(alarm_code, &curl_data);
#ifdef _DEBUG_
    printf("[CURL] curl_data: %s\n", curl_data);
#endif

    curl_global_init(CURL_GLOBAL_DEFAULT); 

    curl = curl_easy_init();
    if (curl == NULL)
    {
#ifdef _DEBUG_
        printf("[CURL] curl_easy_init() failed: %s\n", curl_easy_strerror(res));
#endif
        curl_global_cleanup();  
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, curl_data);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); 
    curl_easy_setopt(curl, CURLOPT_VERBOSE,    0L);   

    res = curl_easy_perform(curl);

#ifdef _DEBUG_
    if (res != CURLE_OK)
    {
        printf("[CURL] curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    else
    {
        printf("[CURL] curl_easy_perform() OK\n");
    }
#endif

    curl_easy_cleanup(curl);
    free(chunk.memory); 
    curl_global_cleanup();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void pd_send_sensor_data(int co, int co2, int temp, int voc)
{
    CURL     *curl;
    CURLcode res;
    char curl_data[256];

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  
    chunk.size = 0;

    memset(&curl_data, 0x00, sizeof(curl_data));

    curl_global_init(CURL_GLOBAL_DEFAULT); 
    curl = curl_easy_init();

    if (curl == NULL)
    {
#ifdef _DEBUG_
        printf("[CURL] curl_easy_init() failed: %s\n", curl_easy_strerror(res));
#endif
        curl_global_cleanup(); 
        return;
    }

    (void) pd_get_curl_data(0, &curl_data);
    curl_easy_setopt(curl, CURLOPT_URL, curl_data);

    sprintf(curl_data, "IP: %s, CO: %d, CO2: %d, TEMP: %d, VOC: %d\n", MY_IP_ADDR, co, co2, temp, voc);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curl_data);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);  
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);    

    res = curl_easy_perform(curl);

#ifdef _DEBUG_
    if (res != CURLE_OK)
    {
        printf("[CURL] curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    else
    {
        printf("[CURL] curl_easy_perform() OK\n");
    }
#endif

    curl_easy_cleanup(curl);
    free(chunk.memory); 
    curl_global_cleanup();
}

