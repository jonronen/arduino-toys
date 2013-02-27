MCU ?= atmega328p

ARDUINO_FILES ?= /usr/share/arduino
ARDUINO_SRCS = $(ARDUINO_FILES)/hardware/arduino/cores/arduino

ARDUINO_PORT ?= /dev/ttyACM0


GPP_FLAGS := -c -g -O3 -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=16000000L -DARDUINO=101 -I$(ARDUINO_SRCS) -I$(ARDUINO_FILES)/hardware/arduino/variants/standard -I$(ARDUINO_FILES)/libraries/SD/utility


sd_test.hex: sd_test.elf
	avr-objcopy -O ihex -R .eeprom sd_test.elf sd_test.hex

sd_test.elf: sd_test.o main.o wiring.o wiring_analog.o wiring_digital.o HardwareSerial.o Sd2Card.o Print.o random.o engine.o filesystem.o
	avr-gcc -O3 -Wl,--gc-sections -mmcu=$(MCU) -o sd_test.elf sd_test.o main.o wiring.o wiring_analog.o wiring_digital.o HardwareSerial.o Sd2Card.o Print.o engine.o random.o filesystem.o -lm

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

filesystem.o: filesystem.cpp
	avr-g++ $(GPP_FLAGS) filesystem.cpp

sd_test.o: sd_test.cpp
	avr-g++ $(GPP_FLAGS) sd_test.cpp

upload: sd_test.hex
	$(ARDUINO_FILES)/hardware/tools/avrdude -C$(ARDUINO_FILES)/hardware/tools/avrdude.conf -v -p$(MCU) -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:sd_test.hex:i

clean:
	rm -f *.o *.hex *.elf

