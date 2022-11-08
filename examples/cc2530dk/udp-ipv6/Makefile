DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

HAVE_BANKING=1

app?=server

# PROJECT_SOURCEFILES += ping6.c

# CONTIKI_PROJECT = $(app)
CONTIKI_PROJECT = server client

# DEFINES+=STARTUP_CONF_VERBOSE=1
ifeq ($(app),client)
	# DEFINES+=MODELS_CONF_CC2531_USB_STICK=1 
	DEFINES+=UART0_ALTERNATIVE_2
else
	# DEFINES+=MODELS_CONF_CC2531_USB_STICK=1
	DEFINES+=UART0_ALTERNATIVE_2
endif

all: $(CONTIKI_PROJECT)

.PHONY: flash
flash:
	@cc-tool -ew $(app).hex

CONTIKI = ../../..
CONTIKI_WITH_IPV6 = 1
# needed for rimestats
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
