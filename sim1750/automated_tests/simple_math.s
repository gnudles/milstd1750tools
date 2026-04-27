	name simple_math
	normal
	org 0x0000
	jc 15, 0x0100

	org 0x0100
start
	lim	r1, 0x0000
	st	r1, 0x2000

    ; simple addition
    lim r2, 0x1111
    lim r3, 0x2222
    ar  r2, r3

    ; check if result is 0x3333
    lim r4, 0x3333
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
