$(BINDIR)/chost.exe: \
		$(OBJDIR)/examples/chost.c.o \
		$(OBJDIR)/examples/exports/hexports.c.o \
		$(OBJDIR)/examples/exports/dos_putfs.S.o \
		$(BINDIR)/libandrea-host.a
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ -li86

$(BINDIR)/cpphost.exe: \
		$(OBJDIR)/examples/cpphost.cpp.o \
		$(OBJDIR)/examples/exports/hexports.c.o \
		$(OBJDIR)/examples/exports/dos_putfs.S.o \
		$(BINDIR)/libandrea-host++.a \
		$(BINDIR)/libandrea-host.a
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ -li86 -lstdc++

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
