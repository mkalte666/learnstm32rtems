PGM=${ARCH}/hello.exe

# optional managers required


MANAGERS=all

# # C source names

CSRCS = test.c
COBJS = $(CSRCS:%.c=${ARCH}/%.o)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

all:    ${ARCH} $(PGM)
$(PGM): $(OBJS)
	$(make-exe)
