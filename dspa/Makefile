## Makefile

CC       = gcc
CFLAGS   = 
DFLAGS   = -D_DEBUG_ 
DFLAGS  += -D_PIGPIO_
//DFLAGS  += -D_DSPA_ON
//DFLAGS  += -D_NEW_MAIN_BOARD_
INCLUDE  = -I./ -I./include -I../include -I/usr/include -I/usr/local/include -I/usr/include/libxml2

CDFLAGS  = $(CFLAGS) $(DFLAGS) $(INCLUDE)

LIBS     = -lm -lpthread -lpigpiod_if2 -lwiringPi -lcurl -lxml2
LDFLAGS  = -L/usr/local/lib -L/lib -L/usr/lib
LDFLAGS += $(LIBS)



APPL = $(HOME)/ICT_control

## Target info
TARGET   = dspa

OBJECT   = dspa_main.o \
	dspa_function.o \
	dspa_status.o \
	dspa_curl.o

SOURCE   = $(OBJECT:.o=.c)

.c.o      :
	$(CC) $(CDFLAGS) -c $<

all       : $(TARGET) cp

$(TARGET) :$(OBJECT)
	$(CC) -o $@ $(OBJECT) $(LDFLAGS)

clean    :
	rm -f *.o  $(TARGET) $(TOOL)

cp        :
	#cp $(TARGET) -f ../bin/$(TARGET)
	sudo cp $(TARGET) -f /opt/dspa/
	sudo cp CurlConfig.xml -f /opt/dspa/
