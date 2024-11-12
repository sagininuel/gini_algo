#ifndef _COMM_DEFINE_H_
#define _COMM_DEFINE_H_

#define  PACKED  __attribute__ ((__packed__))

#define IP_TO_STRING(ip)  (ip&0xff000000)>>24, (ip&0x00ff0000)>>16, (ip&0x0000ff00)>>8, (ip&0x000000ff)

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

typedef struct 
{
    int co2;
    int temp;
    int voc;
} co2_sensor_t;

#define BAUD_RATE   9600
#define DEVICE_PATH "/dev/ttyUSB0"
#define LOG_PATH    "/tmp/app_log/app.log"

#define MAX_IPC_BUFFER            1024
#define HIGH                      1
#define LOW                       0

#define ID_NONE                   0 
#define ID_TEMP                   1
#define ID_CO                     2
#define ID_CO2                    3
#define ID_VOC                    4

#ifdef _NEW_MAIN_BOARD_  //////////////
#define NUM_OUT_GPIO_SHUTTER      9
#define NUM_OUT_GPIO_DSPA1        6
#define NUM_OUT_GPIO_DSPA2        5
#define NUM_OUT_GPIO_DSPA3        11
#define NUM_OUT_GPIO_FIRE_CONTROL 10

#define NUM_IN_GPIO_FIRE_ALARM    19
#define NUM_IN_GPIO_HEAT_SENSOR   12
#define NUM_IN_GPIO_MANUAL_SWITCH 26

#else // _OLD_MAIN_BOARD_  //////////////

//#define NUM_OUT_GPIO_DSPA1        16
//#define NUM_OUT_GPIO_DSPA2        17
//#define NUM_OUT_GPIO_SHUTTER      18
//#define NUM_OUT_GPIO_DSPA3        21

#define NUM_OUT_GPIO_DSPA1        18
#define NUM_OUT_GPIO_DSPA2        21
#define NUM_OUT_GPIO_SHUTTER      16
#define NUM_OUT_GPIO_DSPA3        17

#define NUM_OUT_GPIO_FIRE_CONTROL 23

#define NUM_IN_GPIO_FIRE_ALARM    12
#define NUM_IN_GPIO_HEAT_SENSOR   14
#define NUM_IN_GPIO_MANUAL_SWITCH 15

#endif // _NEW_MAIN_BOARD_  /////////////

#define NUM_TIME_INTERVAL_5S      5
#define NUM_TIME_INTERVAL_10S     10
#define NUM_TIME_INTERVAL_15S     15
#define NUM_TIME_INTERVAL_30S     30
#define NUM_TIME_INTERVAL_60S     60

#define HUMI_MIN                  20
#define HUMI_MAX                  80

#define TEMP_NOR_LEVEL            28
#define TEMP_ANN_LEVEL            70
#define TEMP_1ST_LEVEL            100
#define TEMP_2ND_LEVEL            120
#define TEMP_3RD_LEVEL            200

#define CO_NOR_LEVEL              10
#define CO_ANN_LEVEL              50
#define CO_1ST_LEVEL              100
#define CO_2ND_LEVEL              150
#define CO_3RD_LEVEL              200

#define CO2_NOR_LEVEL             400
#define CO2_ANN_LEVEL             1000
#define CO2_1ST_LEVEL             5000
#define CO2_2ND_LEVEL             6000
#define CO2_3RD_LEVEL             7000

#define FIRE_ACTIVE               1
#define FIRE_RECEIVE              2

#define FIRE_NOR_LEVEL            0
#define FIRE_ANN_LEVEL            1
#define FIRE_1ST_LEVEL            2
#define FIRE_2ND_LEVEL            3
#define FIRE_3RD_LEVEL            4
#define FIRE_END_LEVEL            5
#define FIRE_RCV_ALARM            6


#define ANN_NOR                   0
#define ANN_FIRE_START            1
#define ANN_SHUTTER               2
#define ANN_DSPA1                 3
#define ANN_DSPA2                 4
#define ANN_DSPA3                 5
#define ANN_FIRE_ING              6
#define ANN_FIRE_END              7
#define ANN_WARNING               20

typedef struct 
{
    char *speaker_ip;
    char *speaker_port;
    char *server_ip;
    char *server_port;
    char *path;
    char *file1;
    char *file2;
    char *file3;
    char *file4;
    char *file5;
    char *file6;
    char *file7;
    char *file8;
    char *file9;
} xml_data_t;


#endif

