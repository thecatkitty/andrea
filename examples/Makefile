HEXPORTS_OBJS = \
	$(OBJDIR)/examples/exports/hexports.c.o \
	$(OBJDIR)/examples/exports/dos_putfs.S.o

CHOST_OBJS = \
	$(OBJDIR)/examples/chost.c.o \
	$(HEXPORTS_OBJS)

CPPHOST_OBJS = \
	$(OBJDIR)/examples/cpphost.cpp.o \
	$(HEXPORTS_OBJS)


$(BINDIR)/chost.exe: \
		$(CHOST_OBJS) \
		$(HOSTLIB_FILE)
	@mkdir -p $(@D)
	$(CC) -o $@ $(CHOST_OBJS) $(HOST_LDFLAGS)

$(BINDIR)/cpphost.exe: \
		$(CPPHOST_OBJS) \
		$(CPPLIB_FILE) \
		$(HOSTLIB_FILE)
	@mkdir -p $(@D)
	$(CC) -o $@ $(CPPHOST_OBJS) -l$(CPPLIB_NAME) $(HOST_LDFLAGS) -lstdc++

$(BINDIR)/module1.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/exports/square.c.o
	@mkdir -p $(@D)
	$(CC) -o $@ $(filter %.o,$^) $(MOD_LDFLAGS)

$(BINDIR)/module2.exe: \
		$(MOD_START) \
		$(BINDIR)/libchost.exe.a \
		$(OBJDIR)/examples/exports/hello.c.o \
		$(OBJDIR)/examples/exports/goodbye.c.o
	@mkdir -p $(@D)
	$(CC) -o $@ $(filter %.o,$^) $(MOD_LDFLAGS) -lchost.exe


$(BINDIR)/lib%.exe.a: $(BINDIR)/%.exe
	libman/libman.py $< -o $@
