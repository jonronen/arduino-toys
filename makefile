#
# Arduino Toys
# Copyright 2012-2013 Jon Ronen-Drori
#
# An Arduino-based platform for children's toys
# with LEDs, push-buttons, and sounds
#
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


MCU ?= atmega328p

ARDUINO_FILES ?= /usr/share/arduino
ARDUINO_SRCS = $(ARDUINO_FILES)/hardware/arduino/cores/arduino

ARDUINO_PORT ?= /dev/ttyACM0


GPP_FLAGS := -c -g -O3 -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=8000000L -DARDUINO=101 -I$(ARDUINO_SRCS) -I$(ARDUINO_FILES)/hardware/arduino/variants/standard -I$(ARDUINO_FILES)/libraries/SD/utility


toys.hex: toys.elf
	avr-objcopy -O ihex -R .eeprom toys.elf toys.hex

toys.elf: toys_main.o main.o wiring.o wiring_analog.o wiring_digital.o Sd2Card.o Print.o random.o engine.o filesystem.o sound.o HardwareSerial.o
	avr-gcc -O3 -Wl,--gc-sections -mmcu=$(MCU) -o toys.elf toys_main.o main.o wiring.o wiring_analog.o wiring_digital.o Sd2Card.o Print.o engine.o random.o filesystem.o sound.o HardwareSerial.o -lm

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

fuses:
	$(ARDUINO_FILES)/hardware/tools/avrdude -C$(ARDUINO_FILES)/hardware/tools/avrdude.conf -v -v -v -p$(MCU) -cstk500v1 -P$(ARDUINO_PORT) -b19200 -U lfuse:w:0xe7:m -U hfuse:w:0xdf:m -U efuse:w:0xfd:m

upload_no_bootloader: toys.hex
	$(ARDUINO_FILES)/hardware/tools/avrdude -C$(ARDUINO_FILES)/hardware/tools/avrdude.conf -v -p$(MCU) -cstk500v1 -P$(ARDUINO_PORT) -b19200 -Uflash:w:toys.hex:i

upload: toys.hex
	$(ARDUINO_FILES)/hardware/tools/avrdude -C$(ARDUINO_FILES)/hardware/tools/avrdude.conf -v -p$(MCU) -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:toys.hex:i

clean:
	rm -f *.o *.hex *.elf

