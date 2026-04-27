	name simple_math
	normal
	org 0x0000
	jc 15, 0x0100

	org 0x0100

	normal
start
	 ; (2^19, r1-r2 counted together)
	lim	r0, 19
	lisp	r2, 1
	; shift 1 by 19
	dslr r1, r0
	 ; r3 becomes 2^19
	eflt	r3, r1
	 ; zero r1
	xorr	r1, r1 
	lisp    r2, 1 
	 ; r6 is 1
	eflt	r6, r1 
	; r6 = 1/(2^19)
	efdr    r6, r3 
	; r3 will be 1
	eflt	r3, r1 
	; r6 = 1 + 1/(2^19)
	efar    r6, r3

e_loop1
	efmr    r6, r6
	soj r0, e_loop1

	; store in address 0x1400 for inspection
	efst r6, 0x1400 
	; subtract true value of e
	efs r6, E_VAL 
	; store in address 0x1403 for inspection
	efst r6, 0x1403 
	; store absolute value into r0-r1 (f32)
	fabs r0, r6 
	; if negative flag is on, we are very close, success
	fc r0, EPS_VAL 
	BGE fail
success
	lim	r1, 0xAAAA
	st	r1, 0x2000
	bpt
fail
	lim	r1, 0xDEAD
	st	r1, 0x2000
	bpt
	org 0x0500
	konst
E_VAL
	dataef	2.718281828
EPS_VAL
	dataef	0.00001
	end start
