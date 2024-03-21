HOSTLIB_NAME = andrea
HOSTLIB_FILE = $(BINDIR)/lib$(HOSTLIB_NAME).a

ifdef LOGS
	HOST_LOGS = $(OBJDIR)/host/log.c.o
endif

$(HOSTLIB_FILE): \
		$(OBJDIR)/host/load.c.o \
		$(OBJDIR)/host/free.c.o \
		$(OBJDIR)/host/get_procedure.c.o \
		$(OBJDIR)/host/get_name.c.o \
		$(HOST_LOGS) \
		$(HOST_LD)
	@mkdir -p $(@D)
	ar -rcs $@ $(filter %.o,$^)

$(HOST_LD): \
		host/andrea-host.ld.patch
	@mkdir -p $(@D)
	patch -i $< -o $@ /usr/ia16-elf/lib/dos-mssl.ld
