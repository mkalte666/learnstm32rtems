PGM=${ARCH}/player.exe
BINOUT=${PGM}.bin

# optional managers required


MANAGERS=all

# # C source names

CSRCS = player.c micromod.c 
COBJS = $(CSRCS:%.c=${ARCH}/%.o)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

all:    ${ARCH} $(PGM)
$(PGM): $(OBJS)
	$(make-exe)

burn: $(PGM)
	$(OBJCOPY) -O binary ${PGM} ${BINOUT}
	st-flash write ${BINOUT} 0x08000000
