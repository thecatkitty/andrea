$(BINDIR)/chost.exe: \
		$(OBJDIR)/examples/chost.c.o \
		$(OBJDIR)/examples/exports/hexports.c.o \
		$(BINDIR)/libandrea-host.a
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ -li86

$(BINDIR)/cpphost.exe: \
		$(OBJDIR)/examples/cpphost.cpp.o \
		$(BINDIR)/libandrea-host.a \
		$(BINDIR)/libandrea-host++.a
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $< -L$(BINDIR) -landrea-host++ -landrea-host -li86 -lstdc++

$(BINDIR)/module1.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/exports/square.c.o
	@mkdir -p $(@D)
	$(CC) $(MOD_LDFLAGS) -o $@ $^ -li86

$(BINDIR)/module2.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/exports/hello.c.o
	@mkdir -p $(@D)
	$(CC) $(MOD_LDFLAGS) -o $@ $^ -li86
