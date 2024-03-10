$(BINDIR)/chost.exe: \
		$(OBJDIR)/examples/chost.c.o \
		$(BINDIR)/libandrea-host.a
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $< -L$(BINDIR) -landrea-host -li86

$(BINDIR)/cpphost.exe: \
		$(OBJDIR)/examples/cpphost.cpp.o \
		$(BINDIR)/libandrea-host.a \
		$(BINDIR)/libandrea-host++.a
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $< -L$(BINDIR) -landrea-host++ -landrea-host -li86 -lstdc++

$(BINDIR)/module1.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/module1.c.o \
		$(MOD_END)
	@mkdir -p $(@D)
	$(CC) $(MOD_LDFLAGS) -o $@ $^ -li86

$(BINDIR)/module2.exe: \
		$(MOD_START) \
		$(OBJDIR)/examples/module2.c.o \
		$(MOD_END)
	@mkdir -p $(@D)
	$(CC) $(MOD_LDFLAGS) -o $@ $^ -li86
