UNAME := $(shell uname)
ARCH := $(shell uname -m)

# Modify this to match your setup
HADOOP_DIR=/home/hadoop/hadoop-1.2.1
JAVA_DIR=/home/hadoop/jdk1.7.0_45

# Define libraries
# with HDFS
# LIBS=-ljvm -lhdfs -lhadooppipes -lhadooputils -lcrypto -lpthread -lrt

# w/o HDFS
LIBS=-lhadooppipes -lhadooputils -lcrypto -lpthread -lrt

#### Building on Linux for ARM or x86_64 ####
ifeq ($(UNAME), Linux)

### x86_64 ###
ifeq ($(ARCH), x86_64)
HPLATFORM="Linux-amd64-64"
CC=g++
CCOPT=-Wall --std=c++11 -O2 -g3 -m64 -I$(JAVA_DIR)/include -I$(JAVA_DIR)/include/linux -I$(HADOOP_DIR)/src/c++/install/include
LD=$(CC)
LDOPT=-O2 -g -m64 -L$(JAVA_DIR)/jre/lib/amd64/server -L$(HADOOP_DIR)/src/c++/install/lib $(LIBS)
OBJDMP=objdump
STRIP=strip
endif

### ARM ###
ifeq ($(ARCH), armv7l) 
CC=g++
CCOPT=-Wall -std=c++11 -O3 -marm -mcpu=cortex-a15 -mtune=cortex-a15 -mfpu=neon -mfloat-abi=hard -ffast-math -fstrict-aliasing -I$(JAVA_DIR)/include -I$(JAVA_DIR)/include/linux -I$(HADOOP_DIR)/src/c++/install/include
LD=$(CC)
LDOPT= -O2 -g -mfloat-abi=hard -L$(JAVA_DIR)/jre/lib/arm/server -L$(HADOOP_DIR)/src/c++/install/lib $(LIBS)
OBJDMP=objdump
STRIP=strip
endif

#### i686 ####
ifeq ($(ARCH), i686)
HPLATFORM="Linux-i386-32"
CC=g++
CCOPT=-Wall -std=c++11 -O2 -g3 -m32 -I$(JAVA_DIR)/include -I$(HADOOP_DIR)/src/c++/libhdfs -I$(HADOOP_DIR)/c++/$(HPLATFORM)/include
LD=$(CC)
LDOPT=-O2 -g $(LIBS)
OBJDMP=objdump
STRIP=strip
endif

endif

all: pi pi-gen

pi: VPROG = pi
pi: PiEstimator.o
	$(LD) $(LDOPT) $^ -o $@ $(LDOPT)
	cp pi pi-$(ARCH)

pi-gen: VPROG = pi-gen
pi-gen: PiInputGenerator.o
	$(LD) $(LDOPT) $^ -o $@ $(LDOPT)
	cp pi-gen pi-gen-$(ARCH)

%.o:	%.cpp
	$(CC) $(CCOPT) -DPROG=\"$(VPROG)\" -DARCH=\"$(ARCH)\" -D$(ARCH) -DSYS=\"$(UNAME)\" -D$(UNAME) -c $^ -o $@

oclean:
	rm -rf *.o

clean:
	rm -rf *.o *.s *-armv7l *-x86_64 *-i386 *-i686

