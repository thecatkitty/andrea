/* Andrea header    */
HIDDEN (__andrea_shdr = .);
/* .signature       */ LONG (0x61657226)
/* .size            */ SHORT (__andrea_ehdr - __andrea_shdr)
/* .num_exports     */ SHORT ((__andrea_eexports - __andrea_sexports) / 2)
/* .num_imports     */ SHORT ((__andrea_eimports - __andrea_simports) / 4)
/* .size_expstrs    */ SHORT (__andrea_eexpstrs - __andrea_sexpstrs)
/* .size_impstrs    */ SHORT (__andrea_eimpstrs - __andrea_simpstrs)
HIDDEN (__andrea_ehdr = .);

/* Export table     */
HIDDEN (__andrea_sexports = .);
    *(.andrea.exports)
HIDDEN (__andrea_eexports = .);

/* Import table     */
HIDDEN (__andrea_simports = .);
    *(.andrea.imports)
HIDDEN (__andrea_eimports = .);

/* Export strings   */
HIDDEN (__andrea_sexpstrs = .);
    *(.andrea.expstrs)
HIDDEN (__andrea_eexpstrs = .);

/* Import strings   */
HIDDEN (__andrea_simpstrs = .);
    *(.andrea.impstrs)
HIDDEN (__andrea_eimpstrs = .);
