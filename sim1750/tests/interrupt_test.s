	name interrupt_test

	normal
	org 0x0000
	jc 15, 0x0100

	org 0x0100
start
	lim	r1, 0x0000       ; initialize fail/success to 0
	st	r1, 0x2000       ; address 0x2000 holds test status

	bex	0                ; trigger branch executive (vector 0x2A)

	; If we didn't branch to int_handler, we failed
	lim	r1, 0xDEAD
	st	r1, 0x2000
	bpt                  ; break point to stop simulation

int_handler
	; Success! We reached the interrupt handler
	lim	r1, 0xAAAA
	st	r1, 0x2000
	bpt                  ; stop simulation

	; Set up area where Linkage Pointers point
	static
	org 0x1000
lp_area	block	3

	; Set up area where Service Pointers point
	konst
	org 0x1010
svp_bex	data	0x0000, 0x0000, int_handler

	; Set up the LP/SVP area
	konst
	org	0x2A
lp_bex	data	lp_area
svp_bex_ptr	data	svp_bex

	end start
