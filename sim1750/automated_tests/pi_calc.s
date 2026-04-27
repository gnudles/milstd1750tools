	name simple_math
	normal
	org 0x0000
	jc 15, 0x0100

	org 0x0100

	normal
start
	efl r15, ONE_VAL ; sets r15-r1 to 1
	efl r2, FOUR_VAL
	xorr r5, r5
	xorr r6, r6
	xorr r7, r7
	lim r12, 512
larg_loop:

	lim r11, 1024
	efl r8, ZERO_VAL
	xio r8, 0x400B
loop:

	efl r2, FOUR_VAL
	efdr r2, r15
	efar r8, r2	
	efa r15, TWO_VAL
	efl r2, FOUR_VAL
	efdr r2, r15
	efsr r8, r2
	efa r15, TWO_VAL
	soj r11, loop
	efar r5, r8	
	soj r12, larg_loop
	
	
	; store in address 0x1400 for inspection
	efst r5, 0x1400 
	; subtract true value of e
	efs r5, PI_VAL 
	; store in address 0x1403 for inspection
	efst r5, 0x1403 
	; store absolute value into r0-r1 (f32)
	fabs r0, r5 
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
ZERO_VAL
	dataef 0.0
ONE_VAL
	dataef 1.0
TWO_VAL
	dataef 2.0
FOUR_VAL
	dataef 4.0
PI_VAL
	dataef	3.14159265
EPS_VAL
	dataef	0.000001
	end start
