$(MOD_START): \
		$(OBJDIR)/module/start.S.o \
		$(MOD_LD)
	@mkdir -p $(@D)
	ar -rcs $@ $(filter %.o,$^)

$(MOD_LD): \
		module/andrea-module.ld.patch
	@mkdir -p $(@D)
	patch -i $< -o $@ /usr/ia16-elf/lib/dos-ms.ld
