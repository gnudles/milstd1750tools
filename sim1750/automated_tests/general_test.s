	name inst_test
	normal
	org 0x0000
	jc 15, 0x0100
some_value3: DATA 89ABH
some_value2: DATA 4567H
some_value1: DATA 0123H
value_ref: 
	DATA some_value1
	DATA some_value2
	DATA some_value3
	org 0x0100
start
	lim	r1, 0x1101
	lim	r0, 0x018F
	xorr r1,r0
	cim r1, 0x108E
	bnz fail
	xbr r1
	bge fail
	xwr r1, r0
	ble fail
	lisp r3, 2
	; currently r1 is 0x018F
	lubi r1, value_ref, r3
	; now r1 should be 0x0189 (the lower byte of r1, is the higher byte of some_value3)
	cim r1, 0x0189
	bnz fail
	c  r1, some_value3
	ble fail
	c  r1, some_value2
	bge fail
	slc r1, 4
	; now r1 should be 0x1890 
	llb r1, some_value3, r3
	; now r1 should be 0x1823 (the lower byte of r1, is the lower byte of some_value1)
	cim r1, 0x1823
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
