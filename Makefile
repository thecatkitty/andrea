CC = ia16-elf-gcc

CFLAGS = -mcmodel=small -march=i8088 -Os -Iinclude

BINDIR = bin
OBJDIR = obj

build: $(BINDIR)/host.exe $(BINDIR)/module.exe $(BINDIR)/module2.exe


$(BINDIR)/host.exe: host.c $(BINDIR)/libandrea-host.a
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $< -Xlinker -Map=$@.map -L$(BINDIR) -landrea-host -li86

$(BINDIR)/module.exe: start.S module.c functions.c end.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -nostdlib -li86

$(BINDIR)/module2.exe: start.S module.c functions2.c end.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -nostdlib -li86


include host/Makefile


$(OBJDIR)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $^


clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
