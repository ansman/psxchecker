TARGET = PSXChecker
OBJS = main.o  

INCDIR = 
CFLAGS = -G4 -Wall -O2 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
STDLIBS= -losl -lpng -lz \
  -lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgu -lpspaudiolib -lpspaudio -lm
LIBS=$(STDLIBS)


EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSXChecker
PSP_EBOOT_ICON = ICON0.png


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak