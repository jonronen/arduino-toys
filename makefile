MCU ?= atmega328p

ARDUINO_FILES ?= /usr/share/arduino
ARDUINO_SRCS = $(ARDUINO_FILES)/hardware/arduino/cores/arduino

ARDUINO_PORT ?= /dev/ttyACM0


GPP_FLAGS := -c -g -O3 -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=16000000L -DARDUINO=101 -I$(ARDUINO_SRCS) -I$(ARDUINO_FILES)/hardware/arduino/variants/standard -I$(ARDUINO_FILES)/libraries/SD/utility


toys.hex: toys.elf
	avr-objcopy -O ihex -R .eeprom toys.elf toys.hex

toys.elf: toys_main.o main.o wiring.o wiring_analog.o wiring_digital.o Sd2Card.o Print.o random.o engine.o filesystem.o sound.o
	avr-gcc -O3 -Wl,--gc-sections -mmcu=$(MCU) -o toys.elf toys_main.o main.o wiring.o wiring_analog.o wiring_digital.o Sd2Card.o Print.o engine.o random.o filesystem.o sound.o -lm

main.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/main.cpp

wiring.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/wiring.c

wiring_analog.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/wiring_analog.c

wiring_digital.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/wiring_digital.c

HardwareSerial.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/HardwareSerial.cpp

Print.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_SRCS)/Print.cpp

Sd2Card.o:
	avr-g++ $(GPP_FLAGS) $(ARDUINO_FILES)/libraries/SD/utility/Sd2Card.cpp

random.o: random.cpp
	avr-g++ $(GPP_FLAGS) random.cpp

engine.o: engine.cpp
	avr-g++ $(GPP_FLAGS) engine.cpp

sound.o: sound.cpp
	avr-g++ $(GPP_FLAGS) sound.cpp

filesystem.o: filesystem.cpp
	avr-g++ $(GPP_FLAGS) filesystem.cpp

toys_main.o: toys_main.cpp
	avr-g++ $(GPP_FLAGS) toys_main.cpp

upload: toys.hex
	$(ARDUINO_FILES)/hardware/tools/avrdude -C$(ARDUINO_FILES)/hardware/tools/avrdude.conf -v -p$(MCU) -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:toys.hex:i

clean:
	rm -f *.o *.hex *.elf

