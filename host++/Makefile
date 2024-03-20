$(BINDIR)/libandrea-host++.a: \
		$(OBJDIR)/host++/host.cpp.o \
		$(OBJDIR)/host++/callf.S.o
	@mkdir -p $(@D)
	ar -rcs $@ $^
