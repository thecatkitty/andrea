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
	$(CC) $(LDFLAGS) -o $@ $(CHOST_OBJS) -L$(BINDIR) -l$(HOSTLIB_NAME) -li86

$(BINDIR)/cpphost.exe: \
		$(CPPHOST_OBJS) \
		$(CPPLIB_FILE) \
		$(HOSTLIB_FILE)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $(CPPHOST_OBJS) -L$(BINDIR) -l$(CPPLIB_NAME) -l$(HOSTLIB_NAME) -li86 -lstdc++

$(BINDIR)/module1.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/exports/square.c.o
	@mkdir -p $(@D)
	$(CC) $(MOD_LDFLAGS) -o $@ $^ -li86

$(BINDIR)/module2.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/exports/hello.c.o \
		$(OBJDIR)/examples/exports/goodbye.c.o
	@mkdir -p $(@D)
	$(CC) $(MOD_LDFLAGS) -o $@ $^ -li86
