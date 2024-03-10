ifdef LOGS
	HOST_LOGS = $(OBJDIR)/host/log.c.o
endif

$(BINDIR)/libandrea-host.a: \
		$(OBJDIR)/host/host.c.o \
		$(HOST_LOGS)
	@mkdir -p $(@D)
	ar -rcs $@ $^

$(BINDIR)/libandrea-host++.a: \
		$(OBJDIR)/host/host.cpp.o
	@mkdir -p $(@D)
	ar -rcs $@ $^
