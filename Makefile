CC = ia16-elf-gcc


build: host.exe module.exe


host.exe: host.c andrea.c
	$(CC) -mcmodel=small -o $@ $^ -Xlinker -Map=$@.map -li86

module.exe: module.c andrea.c
	$(CC) -mcmodel=small -o $@ $^ -Xlinker -Map=$@.map


clean:
	rm *.exe
	rm *.map
