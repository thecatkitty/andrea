$(BINDIR)/andrea-modstart.a: \
		$(OBJDIR)/module/start.S.o \
		$(OBJDIR)/module/init.c.o
	@mkdir -p $(@D)
	ar -rcs $@ $^

$(BINDIR)/andrea-modend.o: \
		$(OBJDIR)/module/end.S.o
	@mkdir -p $(@D)
	cp $< $@
