$(MOD_START): \
		$(OBJDIR)/module/start.S.o \
		$(OBJDIR)/module/init.c.o
	@mkdir -p $(@D)
	ar -rcs $@ $^

$(MOD_END): \
		$(OBJDIR)/module/end.S.o
	@mkdir -p $(@D)
	cp $< $@
