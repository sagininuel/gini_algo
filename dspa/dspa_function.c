#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <mariadb/mysql.h>
#include <pigpiod_if2.h>
#include <termios.h>
#include <fcntl.h>
#include <wiringPi.h>

#include "comm_def.h"

void pd_set_gpio_alt();
int  pd_get_gpio_input(int gpio_no);
int  pd_set_gpio_input(int gpio_no);
int  pd_set_gpio_output(int gpio_no);

int  getTemperature_algorithm();
int  getCo2_algorithm(co2_sensor_t *sensor_d);
int  getAll_algorithm(co2_sensor_t *sensor_d);
int  getCo_algorithm();

int pd_serial_open() ;

uint8_t all_ref_frame1[]   = {0x01, 0x03, 0x00, 0x02, 0x00, 0x06};
uint8_t co2_ref_frame1[]   = {0x01, 0x03, 0x00, 0x02, 0x00, 0x01};
uint8_t voc_ref_frame1[]   = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01};
//uint8_t temp_ref_frame1[]  = {0x01, 0x03, 0x00, 0x0c, 0x00, 0x01};
uint8_t temp_ref_frame1[]  = {0xe4, 0x03, 0x00, 0x0b, 0x00, 0x01};
uint8_t co_ref_frame1[]    = {0x05, 0x03, 0x10, 0x00, 0x00, 0x01};

uint8_t all_ref_frame2[]   = {0x02, 0x03, 0x00, 0x02, 0x00, 0x06};
uint8_t co2_ref_frame2[]   = {0x02, 0x03, 0x00, 0x02, 0x00, 0x01};
uint8_t voc_ref_frame2[]   = {0x02, 0x03, 0x00, 0x06, 0x00, 0x01};
//uint8_t temp_ref_frame2[]  = {0x02, 0x03, 0x00, 0x0c, 0x00, 0x01};
uint8_t co_ref_frame2[]    = {0x06, 0x03, 0x10, 0x00, 0x00, 0x01};

uint8_t all_ref_frame3[]   = {0x03, 0x03, 0x00, 0x02, 0x00, 0x06};
uint8_t co2_ref_frame3[]   = {0x03, 0x03, 0x00, 0x02, 0x00, 0x01};
uint8_t voc_ref_frame3[]   = {0x03, 0x03, 0x00, 0x06, 0x00, 0x01};
//uint8_t temp_ref_frame3[]  = {0x03, 0x03, 0x00, 0x0c, 0x00, 0x01};
uint8_t co_ref_frame3[]    = {0x07, 0x03, 0x10, 0x00, 0x00, 0x01};

uint8_t all_ref_frame4[]   = {0x04, 0x03, 0x00, 0x02, 0x00, 0x06};
uint8_t co2_ref_frame4[]   = {0x04, 0x03, 0x00, 0x02, 0x00, 0x01};
uint8_t voc_ref_frame4[]   = {0x04, 0x03, 0x00, 0x06, 0x00, 0x01};
//uint8_t temp_ref_frame4[]  = {0x04, 0x03, 0x00, 0x0c, 0x00, 0x01};
uint8_t co_ref_frame4[]    = {0x08, 0x03, 0x10, 0x00, 0x00, 0x01};


int system_initial = 0;
int reference_voc  = 0;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void pd_set_gpio_alt()
{

#ifdef _DEBUG_
    printf("START pd_set_gpio_alt() !!!!!!!!!!!!!!\n");
#endif

    pd_set_gpio_input(NUM_IN_GPIO_FIRE_ALARM);
    pd_set_gpio_input(NUM_IN_GPIO_HEAT_SENSOR);     
    pd_set_gpio_input(NUM_IN_GPIO_MANUAL_SWITCH);

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int pd_pigpio_open()
{
    int pi;

    while(1)
    {
        pi = pigpio_start(NULL, NULL);
        if (pi >= 0) break;

        usleep(1000);
    }

    return pi;
}


//////////////////////////////////////////////////////////////////////////////
// GPIO Input  
// Heat       : GPIO 14 - I/O PIN No. 8
// MANUAL     : GPIO 15 - I/O PIN No. 10
// Fire Alarm : GPIO 17 - I/O PIN No. 11
//////////////////////////////////////////////////////////////////////////////
int pd_get_gpio_input(int gpio_no)
{
    int status = 0;
    int pi = pd_pigpio_open();

    set_mode(pi, gpio_no, PI_INPUT);
    status = gpio_read(pi, gpio_no);

#ifdef _DEBUG_
    printf("GPIO%d get %d !!!!!!!!!!!!!!!!\n", gpio_no, gpio_read(pi, gpio_no));
#endif

    pigpio_stop(pi);

    return status;
}

//////////////////////////////////////////////////////////////////////////////
// GPIO Output      
// DSPA#1 : GPIO 12 - I/O PIN No. 32
// DSPA#2 : GPIO 16 - I/O PIN No. 36
// DSPA#3 : GPIO 21 - I/O PIN No. 40
// FIRE   : GPIO 23 - I/O PIN No. 16
//////////////////////////////////////////////////////////////////////////////
int pd_set_gpio_output(int gpio_no)
{
    int pi = pd_pigpio_open();

    int chk_sts = gpio_read(pi, gpio_no);

    set_pull_up_down(pi, gpio_no, PI_PUD_UP);
    set_mode(pi, gpio_no, PI_OUTPUT);
    gpio_write(pi, gpio_no, !chk_sts);

#ifdef _DEBUG_
    printf("GPIO%d set %d !!!!!!!!!!!!!!!!\n", gpio_no, gpio_read(pi, gpio_no));
#endif

    pigpio_stop(pi);

    return 1;
}


//////////////////////////////////////////////////////////////////////////////
// GPIO Input      
// Heat       : GPIO 14 - I/O PIN No. 8
// MANUAL     : GPIO 15 - I/O PIN No. 10
// Fire Alarm : GPIO 17 - I/O PIN No. 11
//////////////////////////////////////////////////////////////////////////////
int pd_set_gpio_input(int gpio_no)
{
    int pi = pd_pigpio_open();


    set_mode(pi, gpio_no, PI_INPUT);
    set_pull_up_down(pi, gpio_no, PI_PUD_DOWN);
    gpio_write(pi, gpio_no, LOW);

#ifdef _DEBUG_
    printf("GPIO%d set %d !!!!!!!!!!!!\n", gpio_no, gpio_read(pi, gpio_no));
#endif

    pigpio_stop(pi);

    return 1;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
unsigned short crc16(const unsigned char *data, size_t length) 
{
    unsigned short crc = 0xFFFF;

    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int get_rs485_data(uint8_t *chk_buff) 
{
    const char buf[8];

    memset(&buf, 0x00, sizeof(buf));

    uint16_t crc = crc16(chk_buff, 6);

    chk_buff[6] =  (crc & 0xFF);
    chk_buff[7] =  (crc >> 8);

    int fd = pd_serial_open();
    if (fd < 0) 
    {
#ifdef _DEBUG_
        printf("serialOpen(%s) Fail: %s\n", DEVICE_PATH, strerror (errno)) ;
#endif
        return 0;
    }

#if 0
#ifdef _DEBUG_
    printf("request RTU = ");
    for (int i = 0; i < 8; i++)
        printf("[0x%02x] ", chk_buff[i]);
    printf("\n");
#endif
#endif

    if (write(fd, chk_buff, 8) <= 0)
    {
#ifdef _DEBUG_
        printf("[%s:%d] Write Fail[ID: 0x%02x]: %s\n", __FILE__, __LINE__, chk_buff[0], strerror (errno));
#endif
        close(fd);
        return 0;
    }
    usleep(100000); // 100 ms

    if (read(fd, buf, 7) <= 0) 
    {
#ifdef _DEBUG_
        printf("[%s:%d] Read Fail[ID: 0x%02x]: %s\n", __FILE__, __LINE__, chk_buff[0], strerror (errno));
#endif
        close(fd);
        return 0;
    }
    close(fd);

    int32_t read_value = (buf[3] * 256) + buf[4];

#ifdef _DEBUG_
    //printf("read_val = %d\n", read_value);
#endif

    if (read_value < 0) read_value = 0;
    
    return (read_value);

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int get_rs485_data_all(uint8_t *chk_buff, co2_sensor_t *sensor_d) 
{
    const char buf[18];

    memset(&buf, 0x00, sizeof(buf));

    uint16_t crc = crc16(chk_buff, 6);

    chk_buff[6] =  (crc & 0xFF);
    chk_buff[7] =  (crc >> 8);

    int fd = pd_serial_open();
    if (fd < 0) 
    {
#ifdef _DEBUG_
        printf("serialOpen(%s) Fail: %s\n", DEVICE_PATH, strerror (errno)) ;
#endif
        return 0;
    }

#if 0
#ifdef _DEBUG_
    printf("request RTU = ");
    for (int i = 0; i < 8; i++)
        printf("[0x%02x] ", chk_buff[i]);
    printf("\n");
#endif
#endif

    if (write(fd, chk_buff, 8) <= 0)
    {
#ifdef _DEBUG_
        printf("[%s:%d] Write Fail[ID: 0x%02x]: %s\n", __FILE__, __LINE__, chk_buff[0], strerror (errno));
#endif
        close(fd);
        return 0;
    }
    usleep(100000); // 100 ms

    if (read(fd, buf, 17) <= 0) 
    {
#ifdef _DEBUG_
        printf("[%s:%d] Read Fail[ID: 0x%02x]: %s\n", __FILE__, __LINE__, chk_buff[0], strerror (errno));
#endif
        close(fd);
        return 0;
    }
    close(fd);

#if 0
    printf("read = ");
    for (int j = 0; j < 19; j++)
        printf("[0x%02x]", buf[j]);
    printf("\n");
#endif

    sensor_d->co2  = (buf[3]  * 256) + buf[4];
    sensor_d->voc  = (buf[7]  * 256) + buf[8];
    sensor_d->temp = ((buf[13] * 256) + buf[14]) / 10.0;

#ifdef _DEBUG_
    printf("temp= %d, CO2= %d, VOC= %d\n", sensor_d->temp, sensor_d->co2, sensor_d->voc);
#endif

    return 1;
    
}

#if 1
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int getTemperature_algorithm()
{
    int temp1 = get_rs485_data(temp_ref_frame1);

#if 0
    int temp2 = get_rs485_data(temp_ref_frame2);

    int temp3 = get_rs485_data(temp_ref_frame3);
#endif

#ifdef _DEBUG_
    printf("[%s:%d] TEMP  = %d\n", __FUNCTION__, __LINE__, temp1);
#endif

    return (temp1 / 10.0);
}
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int getCo2_algorithm(co2_sensor_t *sensor_d)
{
    int read_co2 = 0, read_temp = 0, read_voc = 0;
    co2_sensor_t sensor_1, sensor_2, sensor_3, sensor_4;

#ifdef _DEBUG_
    printf("[%d] getCo2_algorithm() \n", __LINE__);
#endif

    // Sensor No.1 --------------------------------------------
    memset(&sensor_1, 0x00, sizeof(co2_sensor_t));

    sensor_1.co2 = get_rs485_data(co2_ref_frame1);
    if (sensor_1.co2 > 0) read_co2++;
    usleep(100000);

    sensor_1.voc = get_rs485_data(voc_ref_frame1);
    if (sensor_1.voc > 0) read_voc++;
    usleep(100000);

#if 0
    sensor_1.temp = get_rs485_data(temp_ref_frame1) / 10.0;
    if (sensor_1.temp > 0) read_temp++;
    usleep(100000);
#endif

    // Sensor No.2 --------------------------------------------
    memset(&sensor_2, 0x00, sizeof(co2_sensor_t));

    sensor_2.co2 = get_rs485_data(co2_ref_frame2);
    if (sensor_2.co2 > 0) read_co2++;
    usleep(100000);

    sensor_2.voc = get_rs485_data(voc_ref_frame2);
    if (sensor_2.voc > 0) read_voc++;
    usleep(100000);

#if 0
    sensor_2.temp = get_rs485_data(temp_ref_frame2) / 10.0;
    if (sensor_2.temp  > 0) read_temp++;
    usleep(100000);
#endif

#if 0 // +++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Sensor No.3 --------------------------------------------
    memset(&sensor_3, 0x00, sizeof(co2_sensor_t));

    if ((sensor_3.co2 = get_rs485_data(co2_ref_frame3)) > 0) read_co2++;
    usleep(100000);

    if ((sensor_3.temp = get_rs485_data(temp_ref_frame3) / 10.0) > 0) read_temp++;
    usleep(100000);

    // Sensor No.4 --------------------------------------------
    memset(&sensor_4, 0x00, sizeof(co2_sensor_t));

    if ((sensor_4.co2 = get_rs485_data(co2_ref_frame4)) > 0) read_co2++;
    usleep(100000);

    if ((sensor_4.temp = get_rs485_data(temp_ref_frame4) / 10.0) > 0) read_temp++;
    usleep(100000);

#endif // +++++++++++++++++++++++++++++++++++++++++++++++++++++++

    memset(sensor_d, 0, sizeof(co2_sensor_t));

#if 1 //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (read_co2 > 0)
        sensor_d->co2   = (sensor_1.co2   + sensor_2.co2)   / read_co2;
    //if (read_temp > 0)
        //sensor_d->temp  = (sensor_1.temp  + sensor_2.temp)  / read_temp;
    if (read_voc > 0)
        sensor_d->voc  = (sensor_1.voc  + sensor_2.voc)  / read_voc;
#else //++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (read_co2 > 0)
        sensor_d->co2   =
            (sensor_1.co2   + sensor_2.co2   + sensor_3.co2   + sensor_4.co2)   / read_co2;
    //if (read_temp > 0)
        //sensor_d->temp  =
            //(sensor_1.temp  + sensor_2.temp  + sensor_3.temp  + sensor_4.temp)  / read_temp;
    if (read_voc > 0)
        sensor_d->voc  =
            (sensor_1.voc  + sensor_2.voc  + sensor_3.voc  + sensor_4.voc)  / read_voc;
#endif //++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef _DEBUG_
    printf("CO2= %d, VOC= %d\n", sensor_d->co2, sensor_d->voc);
#endif

    if (system_initial < 10)
    {
        reference_voc  += sensor_d->voc;
        system_initial++;

        if (system_initial >= 10)
        {
            reference_voc = reference_voc / system_initial;
#ifdef _DEBUG_
            printf("first_VOC = %d\n", reference_voc);
#endif
            log_write("first_VOC = %d\n", reference_voc);
        }

        sensor_d->voc  = 1;

        return 0;
    }


    if (sensor_d->voc <= 0) sensor_d->voc = 1;
    else
        sensor_d->voc  = (sensor_d->voc  / reference_voc) ;
    
#ifdef _DEBUG_
    printf("[%%] CO2= %d, VOC= %d\n", sensor_d->co2, sensor_d->voc);
#endif

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int getAll_algorithm(co2_sensor_t *sensor_d)
{
    int read_all = 0;
    co2_sensor_t sensor_1, sensor_2;

#ifdef _DEBUG_
    printf("[%d] getall_algorithm() \n", __LINE__);
#endif

    // Sensor No.1 --------------------------------------------
    memset(&sensor_1, 0x00, sizeof(co2_sensor_t));

    int rslt = get_rs485_data_all(all_ref_frame1, &sensor_1);
    if (rslt > 0) read_all++;
    usleep(100000);

    // Sensor No.2 --------------------------------------------
    memset(&sensor_2, 0x00, sizeof(co2_sensor_t));

    rslt = get_rs485_data_all(all_ref_frame2, &sensor_2);
    if (rslt > 0) read_all++;
    usleep(100000);

    memset(sensor_d, 0, sizeof(co2_sensor_t));

    if (read_all > 0)
    {
        sensor_d->co2  = (sensor_1.co2  + sensor_2.co2)  / read_all;
        sensor_d->temp = (sensor_1.temp + sensor_2.temp) / read_all;
        sensor_d->voc  = (sensor_1.voc  + sensor_2.voc)  / read_all;
    }

#ifdef _DEBUG_
    //printf("temp= %d, CO2= %d, VOC= %d\n", sensor_d->temp, sensor_d->co2, sensor_d->voc);
#endif

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int getCo_algorithm()
{
    int read_co = 0, co_avg = 0;
    int co1= 0, co2= 0, co3= 0, co4= 0;
#ifdef _DEBUG_
    printf("[%d] getCo_algorithm() \n", __LINE__);
#endif

    if ((co1 = get_rs485_data(co_ref_frame1)) > 0) read_co++;
    usleep(100000);

    if ((co2 = get_rs485_data(co_ref_frame2)) > 0) read_co++;
    usleep(100000);

#if 0 //+++++++++++++++++++++++++++++++++++++++++++++++++++
    if ((co3 = get_rs485_data(co_ref_frame3)) > 0) read_co++;
    usleep(100000);

    if ((co4 = get_rs485_data(co_ref_frame4)) > 0) read_co++;
#endif //+++++++++++++++++++++++++++++++++++++++++++++++++++

    if (read_co > 0)
        co_avg = (co1 + co2 + co3 + co4) / read_co;

#ifdef _DEBUG_
    printf("(co_1: %d, co_2: %d) = %d\n", co1, co2, co_avg);
#endif

    return(co_avg);

}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int pd_serial_open() 
{
    int ttyfd;
    struct termios tty;

    ttyfd = open(DEVICE_PATH, O_RDWR | O_NOCTTY | O_SYNC);
    if (ttyfd < 0) 
    {
        return -1;
    }

    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(ttyfd, &tty) != 0) 
    {
        close(ttyfd);
        return -1;
    }

    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    tty.c_cflag     = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag    &= ~IGNBRK;
    tty.c_lflag     = 0;
    tty.c_oflag     = 0;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 2;

    if (tcsetattr(ttyfd, TCSANOW, &tty) != 0) 
    {
        close(ttyfd);
        return -1;
    }

    return ttyfd;
}
