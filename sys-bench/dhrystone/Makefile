UNAME := $(shell uname)
ARCH := $(shell uname -m)

#### Building on Linux for ARM or x86_64 ####
ifeq ($(UNAME), Linux)

### x86_64 ###
ifeq ($(ARCH), x86_64)
CC=gcc
CCOPT=-Wall -O3 -g3 -m64
CCNOPT=-Wall -O0 -g3 -m64
LD=$(CC)
LDOPT=-O3 -g -lrt -m64
LDNOPT=-O0 -g -lrt -m64
OBJDMP=objdump
STRIP=strip
endif

### ARM ###
ifeq ($(ARCH), armv7l) 
CC=gcc
# CCOPT=-Wall -O3 -marm -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -mfloat-abi=hard -ffast-math -fstrict-aliasing
# CCOPT=-Wall -O3 -marm -mcpu=cortex-a15 -mtune=cortex-a15 -mfpu=neon -mfloat-abi=hard -ffast-math -fstrict-aliasing
CCOPT=-Wall -O3 -marm -mfpu=neon -mfloat-abi=hard -ffast-math -fstrict-aliasing -mcpu=cortex-a15
CCNOPT=-Wall -O0 -marm -mfpu=neon -mfloat-abi=hard -ffast-math -fstrict-aliasing -mcpu=cortex-a15
LD=$(CC)
LDOPT= -O3 -g -mfloat-abi=hard -lrt
LDNOPT= -O0 -g -mfloat-abi=hard -lrt
OBJDMP=objdump
STRIP=strip
endif

#### i686 ####
ifeq ($(ARCH), i686)
CC=gcc
CCOPT=-Wall -O3 -g3 -m32
CCNOPT=-Wall -O0 -g3 -m32
LD=$(CC)
LDOPT=-O3 -g -lrt
LDOPT=-O0 -g -lrt
OBJDMP=objdump
STRIP=strip
endif

endif

all: nopt

opt: dhrystone-opt-$(ARCH)

nopt: dhrystone-nopt-$(ARCH)

dhrystone-opt-$(ARCH): VPROG = flopbench
dhrystone-opt-$(ARCH): oclean dhry_1-opt.o dhry_2-opt.o
	$(LD) dhry_1-opt.o dhry_2-opt.o -o $@ $(LDOPT)
	#$(STRIP) $@
	cp dhrystone-opt-$(ARCH) dhrystone-$(ARCH)

dhrystone-nopt-$(ARCH): VPROG = flopbench
dhrystone-nopt-$(ARCH): oclean dhry_1-nopt.o dhry_2-nopt.o
	$(LD) dhry_1-nopt.o dhry_2-nopt.o -o $@ $(LDOPT)
	#$(STRIP) $@
	cp dhrystone-nopt-$(ARCH) dhrystone-$(ARCH)

%-opt.o: %.c
	$(CC) $(CCOPT) -DPROG=\"$(VPROG)\" -DARCH=\"$(ARCH)\" -D$(ARCH) -DSYS=\"$(UNAME)\" -D$(UNAME) -c $^ -o $@

%-nopt.o: %.c
	$(CC) $(CCNOPT) -DPROG=\"$(VPROG)\" -DARCH=\"$(ARCH)\" -D$(ARCH) -DSYS=\"$(UNAME)\" -D$(UNAME) -c $^ -o $@
	
oclean:
	rm -rf *.o

clean:
	rm -rf *.o *-armv7l *-x86_64 *-i386 *-i686


