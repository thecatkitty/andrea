$(BINDIR)/chost.exe: \
		$(OBJDIR)/examples/chost.c.o \
		$(BINDIR)/libandrea-host.a
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $< -L$(BINDIR) -landrea-host -li86

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
