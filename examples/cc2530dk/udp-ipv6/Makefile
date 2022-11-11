DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

HAVE_BANKING=1

app?=lowusb2serial

CONTIKI_PROJECT = $(app)

# DEFINES+=STARTUP_CONF_VERBOSE=1
DEFINES+=MODELS_CONF_CC2531_USB_STICK=1
DEFINES+=UART0_ALTERNATIVE_2 UART0_CONF_WITH_INPUT

all: $(CONTIKI_PROJECT)

.PHONY: flash
flash:
	@cc-tool -ew $(app).hex

CONTIKI = ../../..
CONTIKI_WITH_IPV6 = 1
# needed for rimestats
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
