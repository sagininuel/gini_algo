#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <pthread.h>

extern int  pd_check_my_ip_address();
extern void *ps_status_start(void * data);
extern void *ps_heat_start(void * data);

char MY_IP_ADDR[16];

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void pd_dspa_thread_start()
{
    pthread_t p_status_thread;
    //pthread_t p_heat_thread;
    int thread_id;
    int status;

    thread_id = pthread_create(&p_status_thread, NULL, ps_status_start, (void *)NULL);
    if (thread_id < 0)
    {
#ifdef _DEBUG_
        printf("DSPA Status thread create error \n");
#endif
        exit(0);
    }

#if 0
    thread_id = pthread_create(&p_heat_thread, NULL, ps_heat_start, (void *)NULL);
    if (thread_id < 0)
    {
#ifdef _DEBUG_
        printf("VOLT thread create error \n");
#endif
        exit(0);
    }
#endif

    pthread_join(p_status_thread, (void **)&status);
    //pthread_join(p_heat_thread, (void **)&status);

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{

#ifdef _DEBUG_
    printf("[%s:%d] START DSPA main\n", __FILE__, __LINE__);
#endif

    pd_check_my_ip_address();
    printf("[%s:%d] MY_IP_ADDR: %s\n", __FILE__, __LINE__, MY_IP_ADDR);

    pd_dspa_thread_start();

    return 0;
}

