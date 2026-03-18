	name simple_logic
	normal
	org 0x0000
	jc 15, 0x0100

	org 0x0100
start
	lim	r1, 0x0000
	st	r1, 0x2000

    ; simple NAND (NR)
    ; ~(0xFFFF & 0x5555) = ~(0x5555) = 0xAAAA
    lim r2, 0xFFFF
    lim r3, 0x5555
    nr  r2, r3

    ; check if result is 0xAAAA
    lim r4, 0xAAAA
    cr  r2, r4
    bnz fail

success
	lim	r1, 0xAAAA
	st	r1, 0x2000
	bpt

fail
	lim	r1, 0xDEAD
	st	r1, 0x2000
	bpt

	end start
