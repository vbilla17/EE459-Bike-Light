# DEVICE     = atmega328p # Uncomment for ATmega328P
DEVICE     = atmega328pb # Uncomment for ATmega328PB
CLOCK      = 7372800UL
PROGRAMMER = -c usbtiny -P usb
SRC_DIR    = src
INC_DIR    = include
BUILD_DIR  = build
SOURCES    = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS    = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0xe0:m

AVRDUDE    = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE    = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -I$(INC_DIR)

# Symbolic targets:
.PHONY: all flash fuse install clean disasm cpp

all: $(BUILD_DIR)/main.hex

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(COMPILE) -c $< -o $@

flash: all
	$(AVRDUDE) -U flash:w:$(BUILD_DIR)/main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

clean:
	rm -rf $(BUILD_DIR)

# File targets:
$(BUILD_DIR)/main.elf: $(OBJECTS)
	$(COMPILE) -o $@ $(OBJECTS)

$(BUILD_DIR)/main.hex: $(BUILD_DIR)/main.elf
	rm -f $@
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(DEVICE) $<

# Targets for code debugging and analysis:
disasm: $(BUILD_DIR)/main.elf
	avr-objdump -d $<

cpp:
	$(COMPILE) -E $(SRC_DIR)/main.c
