MICRONUCLEUS=~/Projects/micronucleus/commandline/micronucleus --run
BAUDRATE=
PORT=
CPU_TYPE=attiny85
CPU_FREQ=16000000ULL
TARGET_ELF=main.elf
TARGET_HEX=main.hex

######

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
OBJECTS += $(patsubst %.cpp, %.o, $(wildcard *.cpp))

######

CC = avr-gcc
CXX = avr-g++
LD = $(CC)
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

INCLUDE=/usr/local/CrossPack-AVR/avr/include
CFLAGS = -c -std=gnu99 -Os -Wall -ffunction-sections -fdata-sections -mmcu=$(CPU_TYPE) -I $(INCLUDE) -DF_CPU=$(CPU_FREQ)
CXFLAGS = -c -Os -Wall -ffunction-sections -fdata-sections -mmcu=$(CPU_TYPE) -I $(INCLUDE) -DF_CPU=$(CPU_FREQ)
LDFLAGS = -Os -mmcu=$(CPU_TYPE) -ffunction-sections -fdata-sections -Wl,--gc-sections
OBJCOPYFLAGS = -O ihex -R .eeprom
AVRDUDEFLAGS = -C $(LIBAVRUTIL)/resources/avrdude.conf -p $(CPU_TYPE) -c $(PROGRAMMER) -b $(BAUDRATE) -P $(PORT) -U flash:w:$(TARGET_HEX):i
RMFLAGS = -rf

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) $(OBJCOPYFLAGS) $^ $@

$(TARGET_ELF): $(OBJECTS) $(LIBS)
	$(LD) $(LDFLAGS) -o $@ $^

all: $(TARGET_HEX)

install: $(TARGET_HEX)
	$(MICRONUCLEUS) $<

clean:
	$(RM) $(RMFLAGS) $(TARGET_ELF) $(TARGET_HEX) $(OBJECTS) *~

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXFLAGS) -o $@ $^
