DEVICE     = atmega328p
CLOCK      = 16000000
PROGRAMMER = -c usbtiny -P usb
SRC_DIR    = src
INC_DIR    = include
BUILD_DIR  = build
SOURCES    = $(SRC_DIR)/main.c $(INC_DIR)/uart.c $(INC_DIR)/software_serial.c $(INC_DIR)/gps.c
OBJECTS    = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0xe0:m

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -I$(INC_DIR)

# Symbolic targets:
all:	main.hex

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(COMPILE) -c $< -o $@

$(BUILD_DIR)/%.o: $(INC_DIR)/%.c | $(BUILD_DIR)
	$(COMPILE) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

flash:	all
	$(AVRDUDE) -U flash:w:$(BUILD_DIR)/main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

clean:
	rm -rf $(BUILD_DIR)

# File targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o $(BUILD_DIR)/main.elf $(OBJECTS)

main.hex: main.elf
	rm -f $(BUILD_DIR)/main.hex
	avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.hex
	avr-size --format=avr --mcu=$(DEVICE) $(BUILD_DIR)/main.elf

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d $(BUILD_DIR)/main.elf

cpp:
	$(COMPILE) -E $(SRC_DIR)/main.c
