#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <libxml/parser.h>

#include "comm_def.h"

    
void *ps_status_start(void *data);

extern void pd_set_gpio_alt();
extern int  getTemperature_algorithm();
extern int  getCo2_algorithm(co2_sensor_t *sensor_d);
extern int  getAll_algorithm(co2_sensor_t *sensor_d);
extern int  getCo_algorithm();

extern int  pd_get_gpio_input(int gpio_no);
extern int  pd_set_gpio_output(int gpio_no);

extern void pd_curl_send_alarm(int alarm_code);
extern void pd_send_sensor_data(int co, int co2, int temp, int voc);

int fire_1st_active  = FIRE_NOR_LEVEL;
int fire_2nd_active  = FIRE_NOR_LEVEL;
int save_manual      = FIRE_NOR_LEVEL;
int save_fire_manual = FIRE_NOR_LEVEL;
uint32_t loop_cnt    = 0;

int first_VOC  = 0, second_VOC  = 0;
int first_TEMP = 0, second_TEMP = 0;
int first_CO   = 0, second_CO   = 0;
int first_CO2  = 0, second_CO2  = 0;
int fire_flag  = false;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void log_write(const char* fmt, ...)
{
    va_list args;
    time_t rawtime;
    struct tm *info;
    char c_time[80];
    char logfile[64];
    char date_str[16];
    char buff[MAX_IPC_BUFFER];

    time( &rawtime );
    info = localtime( &rawtime );
    strftime(c_time,80,"%Y-%m-%d %H:%M:%S ", info);

    bzero(buff, sizeof(buff));

    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    va_end(args);

    bzero(date_str, sizeof(date_str));
    strncpy(date_str, c_time, (size_t)10);

    bzero(logfile, sizeof(logfile));
    sprintf(logfile, "%s.%s", LOG_PATH, date_str);

    FILE *out = fopen(logfile, "a");

    if (out == NULL) return;

    fprintf(out, "[%s] %s", c_time, buff);
    fclose(out);

    return;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int pd_check_VOC_and_TEMP_fire(int curr_VOC, int curr_TEMP)
{
    if (curr_VOC > first_VOC && first_VOC > second_VOC)
    {
        if (curr_VOC >= (4 * first_VOC))
        {
            if (curr_TEMP > first_TEMP && first_TEMP > second_TEMP)
            {
                float chk_TEMP = first_TEMP;

                if (fire_flag) 
                    first_TEMP = 1.6 * chk_TEMP;
                else
                    first_TEMP = 1.3 * chk_TEMP;

                if (curr_TEMP >= first_TEMP)
                {
                    second_VOC  = first_VOC;
                    first_VOC   = curr_VOC;
                    second_TEMP = first_TEMP;
                    first_TEMP  = curr_TEMP;

                    if (fire_flag) 
                    {
                        return FIRE_2ND_LEVEL;
                    }
                    else           
                    { 
                        fire_flag = true;
                        return FIRE_1ST_LEVEL;
                    }
                }
            }
        }
    }

    second_VOC  = first_VOC;
    first_VOC   = curr_VOC;
    second_TEMP = first_TEMP;
    first_TEMP  = curr_TEMP;
    fire_flag   = false;

    return FIRE_NOR_LEVEL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int pd_check_VOC_and_CO_fire(int curr_VOC, int curr_CO)
{
    if (curr_VOC > first_VOC && first_VOC > second_VOC)
    {
        if (curr_VOC >= (4 * first_VOC))
        {
            if (curr_CO > first_CO && first_CO > second_CO)
            {
                float chk_CO = first_CO;

                if (fire_flag) 
                    first_CO = 1.6 * chk_CO;
                else
                    first_CO = 1.3 * chk_CO;

                if (curr_CO >= first_CO)
                {
                    second_VOC = first_VOC;
                    first_VOC  = curr_VOC;
                    second_CO  = first_CO;
                    first_CO   = curr_CO;

                    if (fire_flag) 
                    {
                        return FIRE_2ND_LEVEL;
                    }
                    else           
                    { 
                        fire_flag = true;
                        return FIRE_1ST_LEVEL;
                    }
                }
            }
        }
    }

    second_VOC = first_VOC;
    first_VOC  = curr_VOC;
    second_CO  = first_CO;
    first_CO   = curr_CO;
    fire_flag  = false;

    return FIRE_NOR_LEVEL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int pd_check_VOC_and_CO2_fire(int curr_VOC, int curr_CO2)
{
    if (curr_VOC > first_VOC && first_VOC > second_VOC)
    {
        if (curr_VOC >= (4 * first_VOC))
        {
            if (curr_CO2 > first_CO2 && first_CO2 > second_CO2)
            {
                float chk_CO2 = first_CO2;

                if (fire_flag) 
                    first_CO2 = 1.6 * chk_CO2;
                else
                    first_CO2 = 1.3 * chk_CO2;

                if (curr_CO2 >= first_CO2)
                {
                    second_VOC = first_VOC;
                    first_VOC  = curr_VOC;
                    second_CO2  = first_CO2;
                    first_CO2   = curr_CO2;

                    if (fire_flag) 
                    {
                        return FIRE_2ND_LEVEL;
                    }
                    else           
                    { 
                        fire_flag = true;
                        return FIRE_1ST_LEVEL;
                    }
                }
            }
        }
    }

    second_VOC = first_VOC;
    first_VOC  = curr_VOC;
    second_CO2 = first_CO2;
    first_CO2  = curr_CO2;
    fire_flag  = false;

    return FIRE_NOR_LEVEL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int pd_check_fire()
{
    co2_sensor_t sensor_d;
    int recv_fire = 0, fire_level = 0, fire_co2 = 0, fire_co = 0, fire_temp = 0, fire_manual = 0;

    memset(&sensor_d, 0x00, sizeof(co2_sensor_t));

    int heat_sensor =  pd_get_gpio_input(NUM_IN_GPIO_HEAT_SENSOR);
    int curr_manual =  pd_get_gpio_input(NUM_IN_GPIO_MANUAL_SWITCH);

    int curr_co   =  getCo_algorithm();
    int rslt      =  getCo2_algorithm(&sensor_d);
    sensor_d.temp =  getTemperature_algorithm();
        
    if (curr_manual & save_manual)
    {
        if (save_fire_manual == FIRE_1ST_LEVEL)
            save_fire_manual = fire_manual = FIRE_2ND_LEVEL;
        else
            save_fire_manual = fire_manual = FIRE_1ST_LEVEL;
    }
    else
    {
        save_fire_manual = FIRE_NOR_LEVEL;
    }

    fire_level = pd_check_VOC_and_TEMP_fire(sensor_d.voc, sensor_d.temp);
    fire_level = pd_check_VOC_and_CO_fire(sensor_d.voc, curr_co);
    fire_level = pd_check_VOC_and_CO2_fire(sensor_d.voc, sensor_d.co2);

    save_manual = curr_manual;
                
    if (fire_manual >= FIRE_1ST_LEVEL & fire_manual <= FIRE_2ND_LEVEL)
    {
        fire_level = fire_manual;
        //save_manual = 0;
    }

    if (fire_level >= FIRE_1ST_LEVEL)
    {
        log_write("CO(ppm): %d, CO2(ppm): %d, TEMP: %d, VOC: %d\n", curr_co, sensor_d.co2, sensor_d.temp, sensor_d.voc);
        (void) pd_send_sensor_data(curr_co, sensor_d.co2, sensor_d.temp, sensor_d.voc);
    }
    else
    {
#if 0 // test
        if ((loop_cnt > 45) || (loop_cnt == 0)) // 5 Min
        {
            log_write("CO(ppm): %d, CO2(ppm): %d, TEMP: %d, VOC: %d\n", curr_co,sensor_d.co2, sensor_d.temp, sensor_d.voc);
            (void) pd_send_sensor_data(curr_co, sensor_d.co2, sensor_d.temp, sensor_d.voc);
            loop_cnt = 0;
        }
        loop_cnt++;
#else
        log_write("CO(ppm): %d, CO2(ppm): %d, TEMP: %d, VOC: %d\n", curr_co, sensor_d.co2, sensor_d.temp, sensor_d.voc);
        (void) pd_send_sensor_data(curr_co, sensor_d.co2, sensor_d.temp, sensor_d.voc);
#endif

        if (pd_get_gpio_input(NUM_IN_GPIO_FIRE_ALARM)) fire_level = FIRE_RCV_ALARM;
    }

    return fire_level;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void *ps_status_start(void *data)
{

    printf("START Fire Control System() !!!!!\n");

    (void) wiringPiSetupGpio();
    (void) pd_set_gpio_alt();

    while(1)
    {
        int fire_level = pd_check_fire();

        if (fire_level == FIRE_RCV_ALARM)
        {
            log_write("fire breaks out in the building, and Rolling shutters begin to operate.\n");
            (void) pd_curl_send_alarm(ANN_SHUTTER);

            pd_set_gpio_output(NUM_OUT_GPIO_SHUTTER);
            sleep(1);
            pd_set_gpio_output(NUM_OUT_GPIO_SHUTTER);
        }	
        else if (fire_level == FIRE_1ST_LEVEL)
        {
            if (fire_1st_active == 0)
            {
                log_write("FIRE !!!!!, Transmit fire signal to fire reception unit.\n");
                (void) pd_curl_send_alarm(ANN_FIRE_START);
#if 0
                pd_set_gpio_output(NUM_OUT_GPIO_FIRE_CONTROL);
#endif

                log_write("Rolling shutter operation begins. !!!!!\n");
                (void) pd_curl_send_alarm(ANN_SHUTTER);

                pd_set_gpio_output(NUM_OUT_GPIO_SHUTTER);
                sleep(1);
                pd_set_gpio_output(NUM_OUT_GPIO_SHUTTER);

                log_write("Rolling shutter operation completed. & Fire announcement\n");

                log_write("1'St fire extinguisher operation !!!!!\n");
                (void) pd_curl_send_alarm(ANN_DSPA1);
#ifdef _DSPA_ON_ // test ---------------------------------------
                pd_set_gpio_output(NUM_OUT_GPIO_DSPA1);
#endif

                fire_1st_active = 1;
            }
            else
            {
                log_write("Fire extinguishing announcement !!!!!\n");
                (void) pd_curl_send_alarm(ANN_FIRE_ING);
            }
        }
        else if (fire_level == FIRE_2ND_LEVEL)
        {
            log_write("Fire extinguishing announcement !!!!!\n");
            (void) pd_curl_send_alarm(ANN_FIRE_ING);

            if (fire_2nd_active == 0)
            {
                log_write("2'nd fire extinguisher operation !!!!!\n");
                (void) pd_curl_send_alarm(ANN_DSPA2);
#ifdef _DSPA_ON_ // test ---------------------------------------
                pd_set_gpio_output(NUM_OUT_GPIO_DSPA2);
#endif
                sleep(30);
                (void) pd_curl_send_alarm(ANN_FIRE_ING);
                sleep(30);
                (void) pd_curl_send_alarm(ANN_DSPA3);
#ifdef _DSPA_ON_ // test ---------------------------------------
                pd_set_gpio_output(NUM_OUT_GPIO_DSPA3);
#endif

                fire_2nd_active = 1;
            }
        }
        else if (fire_level == FIRE_NOR_LEVEL)
        {
            if (fire_1st_active)
            {
                log_write("Fire suppression completed announcement !!!!!\n");
                (void) pd_curl_send_alarm(ANN_FIRE_END);
            }

            if (fire_2nd_active)
            {
#ifdef _DSPA_ON_ // test ---------------------------------------
                pd_set_gpio_output(NUM_OUT_GPIO_DSPA3);
                pd_set_gpio_output(NUM_OUT_GPIO_DSPA2);
                pd_set_gpio_output(NUM_OUT_GPIO_DSPA1);
#endif
                fire_2nd_active = 0;
                fire_1st_active = 0;
            }
            else if (fire_1st_active)
            {
#ifdef _DSPA_ON_ // test ---------------------------------------
                pd_set_gpio_output(NUM_OUT_GPIO_DSPA1);
#endif
                fire_1st_active = 0;
            }
        }

        sleep(NUM_TIME_INTERVAL_10S);
    } 
}

