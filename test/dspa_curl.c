#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <time.h>
#include <ifaddrs.h>
#include <netinet/in.h>


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
	   printf("Interface name: %s\t%d\n",ifa->ifa_name,strcmp(ifa->ifa_name, "eth0"));
        if (ifa->ifa_name && (strcmp(ifa->ifa_name, "enp1s0") == 0 ? 0 : 1 && strcmp(ifa->ifa_name, "eth0") == 0 ? 0 : 1)== 0) 
	//if (ifa->ifa_addr == NULL)
		//continue;
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
