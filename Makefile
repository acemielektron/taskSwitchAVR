TARGET = taskSwitch

# Files
EXT_C   = c
EXT_C++ = cpp
EXT_ASM = S

OBJECTS = \
	$(patsubst %.$(EXT_C),%.o,$(wildcard ./src/*.$(EXT_C))) \
	$(patsubst %.$(EXT_C++),%.o,$(wildcard ./src/*.$(EXT_C++))) \
	$(patsubst %.$(EXT_ASM),%.o,$(wildcard ./src/*.$(EXT_ASM)))

#MCU = atmega32u4
MCU = atmega328p
OSC = 16000000UL

CC = avr-gcc
CXX = avr-g++
INCLUDES	= -I ./src  -I /usr/avr/include -I /usr/lib/avr/include 

CFLAGS = -Os -mmcu=$(MCU) -DF_CPU=$(OSC) -Wall $(INCLUDES)
CFLAGS += -ffunction-sections -fdata-sections
#CFLAGS += --save-temps #save temporary files (.s,.i,.ii)
CXXFLAGS = -Os -mmcu=$(MCU) -DF_CPU=$(OSC) -Wall $(INCLUDES)
CXXFLAGS += -DENABLE_SERIAL -D_SIMULAVR_
LDFLAGS = -lm -g -Wl,--gc-sections
ASFLAGS = -mmcu=$(MCU) -Os -DF_CPU=$(OSC)

all: $(TARGET).elf size hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	${CXX} -c $(CXXFLAGS) $< -o $@

%.o: %c 
	${CC} -c $(CFLAGS) $< -o $@

%.o: %S 
	${CC} -c $(ASFLAGS) $< -o $@	

.PHONY : clean, hex, flash, bin, size, all
hex:
	avr-objcopy -O ihex $(TARGET).elf $(TARGET).hex
bin:
	avr-objcopy -O binary $(TARGET).elf $(TARGET).bin
size:
	avr-size --mcu=$(MCU) -C -x $(TARGET).elf
flash:
ifeq	($(MCU),atmega32u4) 
	avrdude -p $(MCU) -c avr109 -P $(PORT) -U flash:w:$(TARGET).hex
else
	avrdude -p $(MCU) -c arduino -P $(PORT) -U flash:w:$(TARGET).hex
endif
clean:
	rm $(TARGET).elf $(OBJECTS)
removetemp:
	find . -name "*.i" -type f -delete
	find . -name "*.ii" -type f -delete
#	find . -name "*.s" -type f -delete
sim:
	simulavr -d atmega328 -f $(TARGET).elf  -R 0x22,- -W 0x20,- -T exit
