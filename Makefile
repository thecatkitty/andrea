CC = ia16-elf-gcc

CFLAGS   = -mcmodel=small -march=i8088 -Os -Iinclude
CXXFLAGS = $(CFLAGS) -Iext/optional/include
LDFLAGS  = -mcmodel=small -march=i8088 -Xlinker -Map=$@.map -L$(BINDIR)

MOD_LDFLAGS = $(LDFLAGS) -nostdlib
MOD_START   = $(BINDIR)/andrea-modstart.a
MOD_END     = $(BINDIR)/andrea-modend.o

BINDIR = bin
OBJDIR = obj


ifdef LOGS
	CFLAGS += -DANDREA_LOGS_ENABLE
endif


build: \
	$(BINDIR)/chost.exe \
	$(BINDIR)/cpphost.exe \
	$(BINDIR)/module1.exe \
	$(BINDIR)/module2.exe


include host/Makefile
include module/Makefile
include examples/Makefile


$(OBJDIR)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJDIR)/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -c $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) -c -o $@ $^


clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
