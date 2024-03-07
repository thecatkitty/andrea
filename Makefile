CC = ia16-elf-gcc

CFLAGS = -mcmodel=small -march=i8088 -Os

BINDIR = out

build: $(BINDIR)/host.exe $(BINDIR)/module.exe $(BINDIR)/module2.exe


$(BINDIR)/host.exe: host.c loader.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -li86

$(BINDIR)/module.exe: start.S module.c functions.c end.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -nostdlib -li86

$(BINDIR)/module2.exe: start.S module.c functions2.c end.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -nostdlib -li86


clean:
	rm -rf $(BINDIR)
