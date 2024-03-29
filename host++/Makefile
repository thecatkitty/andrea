CPPLIB_NAME = andrea++
CPPLIB_FILE = $(LIBDIR)/lib$(CPPLIB_NAME).a

$(CPPLIB_FILE): \
		$(OBJDIR)/host++/host.cpp.o \
		$(OBJDIR)/host++/callf.S.o
	@mkdir -p $(@D)
	ar -rcs $@ $^
