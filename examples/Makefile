HEXPORTS_OBJS = \
	$(OBJDIR)/examples/exports/hexports.c.o \
	$(OBJDIR)/examples/exports/dos_putfs.S.o

HOST_OBJS = \
	$(OBJDIR)/examples/host.c.o \
	$(HEXPORTS_OBJS)


$(BINDIR)/host.exe: \
		$(HOST_OBJS) \
		$(HOSTLIB_FILE)
	@mkdir -p $(@D)
	$(CC) -o $@ $(HOST_OBJS) $(HOST_LDFLAGS)

$(BINDIR)/module1.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/exports/square.c.o
	@mkdir -p $(@D)
	$(CC) -o $@ $(filter %.o,$^) $(MOD_LDFLAGS)

$(BINDIR)/module2.exe: \
		$(MOD_START) \
		$(LIBDIR)/libhost.exe.a \
		$(OBJDIR)/examples/exports/hello.c.o \
		$(OBJDIR)/examples/exports/goodbye.c.o
	@mkdir -p $(@D)
	$(CC) -o $@ $(filter %.o,$^) $(MOD_LDFLAGS) -lhost.exe


$(LIBDIR)/lib%.exe.a: $(BINDIR)/%.exe
	libman/libman.py $< -o $@
