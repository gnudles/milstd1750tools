	name cmp_test
	normal
	org 0x0000
	jc 15, 0x0100

	org 0x0100
start
	lim	r1, 0x0000
	st	r1, 0x2000

cmp	lim	r0, 0x1234
    lim r1, 0x5678
    lim r2, 0x1234
    lim r3, 0x5678
	dcr	r0, r2
	bez success
	br  fail

success
	lim	r1, 0xAAAA
	st	r1, 0x2000
	bpt

fail
	lim	r1, 0xDEAD
	st	r1, 0x2000
	bpt

	end start
