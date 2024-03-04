CC = ia16-elf-gcc

CFLAGS = -mcmodel=small -march=i8088 -Os

BINDIR = out

build: $(BINDIR)/host.exe $(BINDIR)/module.exe


$(BINDIR)/host.exe: host.c andrea.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -li86

$(BINDIR)/module.exe: module.S module.c andrea.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ -Xlinker -Map=$@.map -nostdlib -li86


clean:
	rm -rf $(BINDIR)
