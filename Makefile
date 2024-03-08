CC = ia16-elf-gcc

CFLAGS = -mcmodel=small -march=i8088 -Os -Iinclude

BINDIR = bin
OBJDIR = obj

build: $(BINDIR)/host.exe $(BINDIR)/module.exe $(BINDIR)/module2.exe


$(BINDIR)/host.exe: host.c $(BINDIR)/libandrea-host.a
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $< -Xlinker -Map=$@.map -L$(BINDIR) -landrea-host -li86

$(BINDIR)/module.exe: $(BINDIR)/andrea-modstart.a functions.c $(BINDIR)/andrea-modend.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -nostdlib -li86

$(BINDIR)/module2.exe: $(BINDIR)/andrea-modstart.a functions2.c $(BINDIR)/andrea-modend.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -nostdlib -li86


include host/Makefile
include module/Makefile


$(OBJDIR)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJDIR)/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) -c -o $@ $^


clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
