ifdef LOGS
	MOD_LOGS = $(OBJDIR)/module/log.c.o
endif

$(MOD_START): \
		$(OBJDIR)/module/start.S.o \
		$(OBJDIR)/module/init.c.o \
		$(MOD_LOGS)
	@mkdir -p $(@D)
	ar -rcs $@ $^

$(MOD_END): \
		$(OBJDIR)/module/end.S.o
	@mkdir -p $(@D)
	cp $< $@
