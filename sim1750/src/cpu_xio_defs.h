enum xio_defs
{
    XIO_PO_0YXX       = 0x0000, /* Programmed Output */
    XIO_PI_8YXX       = 0x8000,

    XIO_SMK_2000      = 0x2000, /* Set Interrupt Mask */
    XIO_CLIR_2001     = 0x2001, /* Clear Interrupt Request */
    XIO_ENBL_2002     = 0x2002, /* Enable Interrupts */
    XIO_DSBL_2003     = 0x2003, /* Disable Interrupts */
    XIO_RPI_2004      = 0x2004, /* Reset Pending Interrupt */
    XIO_SPI_2005      = 0x2005, /* Set Pending Interrupt Register */
	XIO_OD_2008       = 0x2008, /* Output Discretes */
    XIO_RNS_200A      = 0x200A, /* Reset Normal Power Up Discrete */
    XIO_WSW_200E      = 0x200E, /* Write Status Word */
    XIO_RMK_A000      = 0xA000, /* Read Interrupt Mask */
    XIO_RIC1_A001     = 0xA001, /* Read Input/Output Interrupt Code, Level 1 */
    XIO_RIC2_A002     = 0xA002, /* Read Input/Output Interrupt Code, Level 2 */
    XIO_RPIR_A004     = 0xA004, /* Read Pending Interrupt Register */
    XIO_RDOR_A008     = 0xA008, /* Read Discrete Output Register */
    XIO_RDI_A009      = 0xA009, /* Read Discrete Input */
    XIO_TPIO_A00B     = 0xA00B, /* Test Programmed Output */
    XIO_RMFS_A00D     = 0xA00D, /* Read Memory Fault Status */
    XIO_RSW_A00E      = 0xA00E, /* Read Status Word */
    XIO_RCFR_A00F     = 0xA00F, /* Read and Clear Fault Register */


    XIO_CO_4000       = 0x4000, /* Console Output */
    XIO_CLC_4001      = 0x4001, /* Clear Console */
    XIO_MPEN_4003     = 0x4003, /* Memory Protect Enable */
    XIO_ESUR_4004     = 0x4004, /* Enable Start Up ROM */
    XIO_DSUR_4005     = 0x4005, /* Disable Start Up ROM */
    XIO_DMAE_4006     = 0x4006, /* Direct Memory Access Enable */
    XIO_DMAD_4007     = 0x4007, /* Direct Memory Access Disable */
    XIO_TAS_4008      = 0x4008, /* Timer A, Start */
    XIO_TAH_4009      = 0x4009, /* Timer A, Halt */
    XIO_OTA_400A      = 0x400A, /* Output Timer A */
    XIO_GO_400B       = 0x400B, /* Trigger Go Indicator */
    XIO_TBS_400C      = 0x400C, /* Timer B, Start */
    XIO_TBH_400D      = 0x400D, /* Timer B, Halt */
    XIO_OTB_400E      = 0x400E, /* Output Timer B */
    XIO_CI_C000       = 0xC000, /* Console Input */
    XIO_RCS_C001      = 0xC001, /* Read Console Status */
    XIO_ITA_C00A      = 0xC00A, /* Input Timer A */
    XIO_ITB_C00E      = 0xC00E, /* Input Timer B */

    XIO_LMP_50XX      = 0x5000, /* Load Memory Protect RAM */
    XIO_WIPR_51XY     = 0x5100, /* Write Instruction Page Register */
    XIO_WOPR_52XY     = 0x5200, /* Write Operand Page Register */
    XIO_RMP_D0XX      = 0xD000, /* Read Memory Protect RAM */
    XIO_RIPR_D1XY     = 0xD100, /* Read Instruction Page Register */
    XIO_ROPR_D2XY     = 0xD200, /* Read Operand Page Register */
};

/*
 from the manual:

0YXX PO		Programmed Output:  This command outputs 16 bits of data
		from RA to a programmed I/O port.  Y may be from 0 through 3.

2000 SMK	Set Interrupt Mask:  This command outputs the 16-bit contents
		of the register RA to the interrupt mask register.  A "1" in
		the corresponding bit position allows the interrupt to occur
		and a "0" prevents the interrupt from occurring except for those
		interrupts that are defined such that they cannot be masked.

2001 CLIR	Clear Interrupt Request:  All interrupts are cleared (i.e., the
		pending interrupt register is cleared to all zeros) and the contents
		of the fault register are reset to zero.

2002 ENBL	Enable Interrupts:  This command enables all interrupts which
		are not masked out.  The enable operation takes place after execution
		of the next instruction.

2003 DSBL	Disable Interrupts:  This command disables all interrupts (except
		those that are defined such that they cannot be disabled) at the
		beginning of the execution of the DSBL instruction.

2004 RPI	Reset Pending Interrupt:  The individual interrupt bit to be reset
		shall be designated in register RA as a right justified four bit
		code.  (0 (Base 16) represents interrupt number 0, F (Base 16)
		represents interrupt number 15).  If interrupt 1 (Base 16) is to
		be cleared, then the contents of the fault register shall also be
		set to zero.

2005 SPI	Set Pending Interrupt Register:  This command ORs the 16-bit
		contents of RA with the pending interrupt register.  If there is a
		one in the corresponding bit position of the interrupt mask (same
		bit set in both the PI and the MK), and the interrupts are enabled,
		then an interrupt shall occur after execution of the next instruction.
		If PI  is set to 1, then N is assumed to be 0 (see paragraph 5.30).
		     5

200E WSW	Write Status Word:  This command transfers the contents of RA to
		the status word.

8YXX PI		Programmed Input:  This command inputs 16 bits of data into RA from
		the programmed I/O
		port.  Y may be from 0 through 3.

A000 RMK	Read Interrupt Mask:  The current interrupt mask is transfered into
		register RA.  The interrupt mask is not altered.

A004 RPIR	Read Pending Interrupt Register:  This command transfers the contents
		of the pending interrupt register into RA.  The pending interrupt
		register is not altered.

A00E RSW 	Read Status Word:  This command transfers the 16-bit status word
		into register RA.  The status word remains unchanged.

A00F RCFR	Read and Clear Fault Register:  This command inputs the 16-bit fault
		register to register RA.  The contents of the fault register are
		reset to zero.  Bit 1 in the pending interrupt register is reset
		to zero.

Optional XIO Command Fields and Mnemonics
-------- --- ------- ------ --- ---------

OYXX PO		Programmed Output:  This command outputs 16 bits of data from
		RA to a programmed I/O port.  Y may be from 0 through 3.

2008 OD		Output Discretes:  This command outputs the 16-bit contents of the
		register RA to the discrete output buffer.  A "1" indicates an "on"
		condition and a "0" indicates an "off" condition.

200A RNS	Reset Normal Power Up Discrete:  This command resets the normal
		power up discrete bit.

4000 CO		Console Output:  The 16-bit contents (2 bytes) of register RA are
		output to the console.  The eight most significant bits (byte) are
		sent first.  If no console is present, then this command is treated
		as a NOP (see page 137).

4001 CLC	Clear Console:  This command clears the console interface.

4003 MPEN	Memory Protect Enable:  This command allows the memory protect
		RAM to control memory protection.

4004 ESUR	Enable Start Up ROM:  This command enables the start up ROM (i.e.,
		the ROM overlays main memory).

4005 DSUR	Disable Start Up ROM:  This command disables the start up ROM.

4006 DMAE	Direct Memory Access Enable:  This command enables direct memory
		access (DMA).

4007 DMAD	Direct Memory Access Disable:  This command disables DMA.

4008 TAS	Timer A, Start:  This command starts timer A from its current state.
		The timer is incremented every 10 microseconds.

4009 TAH	Timer A, Halt:  This command halts timer A at its current state.

400A OTA	Output Timer A:  The contents of register RA are loaded (i.e.,
		jam transfered) into timer A and the timer automatically starts
		operation by incrementing from the loaded timer in steps of ten
		microseconds.  Bit fifteen is the least significant bit and shall
		represent ten microseconds.

400B GO		Trigger Go Indicator:  This command restarts a counter which is
		connected to a discrete output.  The period of time from restart
		to time-out shall be determined by the system requirements.  When
		the Go timer is started, the discrete output shall go high and
		remain high for
		TBD milliseconds, at which time the output shall go low unless
		another GO is executed.  The Go discrete output signal may be
		used as a software fault indicator.

400C TBS	Timer B, Start:  This command starts timer B from its current
		state.  The timer is incremented every 100 microseconds.

400D TBH	Timer B, Halt:  This command halts timer B at its current state.

400E OTB	Output Timer B:  The contents of register RA are loaded (i.e.,
		jam transfered) into timer B and the timer automatically starts
		operation by incrementing from the loaded timer in steps of one
		hundred microseconds.  Bit fifteen is the least significant bit
		and shall represent one hundred microseconds.

50XX LMP 	Load Memory Protect RAM (5000 + RAM address):  This command outputs
		the 16-bit contents of register RA to the memory protect RAM.
		A "1" in a bit provides write protection and a "0" in a bit permits
		writing to the corresponding 1024 word physical memory block.
		The RAM word MSB (bit 0) represents the lowest number block and
		the RAM word LSB (bit 15) represents the highest block (i.e.,
		bit 0 represents locations 0 through 1023 and bit 15 represents
		locations 15360 through 16383 for word zero).  Each word represents
		consecutive 16K blocks of physical memory.  The RAM words of 0
		through 63 apply to processor write protect and words 64 through
		127 apply to DMA write protect.

51XY WIPR	Write Instruction Page Register:  This command transfers the contents
		of register RA to page register Y of the instruction set group X.

52XY WOPR	Write Operand Page Register:  This command transfers the contents
		of register RA to page register Y of the operand set of group X.

8YXX PI		Programmed Input:  This command inputs 16 bits of data into RA
		from the programmed I/O port.  Y may be from 0 through 3.

A001 RIC1	Read Input/Output Interrupt Code, Level 1:  This command inputs
		the contents of the level 1 IOIC register into register RA.  The
		channel number is right justified.

A002 RIC2	Read Input/Output Interrupt Code, Level 2:  This command inputs
		the contents of the level 2 IOIC register into register RA.  The
		channel number is right justified.

A008 RDOR	Read Discrete Output Register:  This command inputs the 16-bit
		discrete output buffer into register RA.

A009 RDI	Read Discrete Input:  This command inputs the 16-bit discrete input
		word into register RA.  A "1" indicates an "on" condition and a
		"0" indicates an "off" condition.

A00B TPIO	Test Programmed Output:  This command inputs the 16-bit contents
		of the programmed output buffer into register RA.  This command
		may be used to test the PIO channel by means of a wrap around test.

A00D RMFS	Read Memory Fault Status:  This command transfers the 16-bit
		contents of the memory fault status register to RA.  The fields
		within the memory fault status register shall delineate memory
		related fault types and shall provide the page register designators
		associated with the designated fault.

C000 CI		Console Input:  This command inputs the 16-bits (2 bytes) from
		the console into register RA.  The eight most significant bits
		of RA shall represent the first byte.


C001 RCS	Read Console Status:  This command inputs the console interface
		status into register RA.  The status is right justified.

C00A ITA	Input Timer A:  This command inputs the 16-bit contents of timer A
		into register RA.  Bit fifteen is the least significant bit and
		represents a time increment of ten microseconds.

C00E ITB	Input Timer B:  This command inputs the 16-bit contents of timer B
		into register RA.  Bit fifteen is the least significant bit and
		represents a time increment of one hundred microseconds.

D0XX RMP	Read Memory Protect RAM (D000 + RAM address):  This command inputs
		the appropriate memory protect word into register RA.  A "1" in
		a bit provides write protection and a "0" in a bit permits writing
		to the corresponding 1024 word physical memory block.  The RAM
		words MSB (bit 0) represents the lowest number block and the RAM
		word LSB (bit 15) represents the highest block (i.e., bit 0
		represents locations 0 through 1023 and bit 15 represents locations
		15360 through 16383 for word zero).  Each word represents consecutive
		16K blocks of physical memory.  The RAM words of 0 through 63
		apply to processor write protect and words 64 through 127 apply
		to DMA write protect.

D1XY RIPR	Read Instruction Page Register:  This command transfers the 16-bit
		contents of the page register Y of the instruction set of group X
		to register RA.

D2XY ROPR	Read Operand Page Register:  This command transfers the 16-bit contents
		of page register Y of the operand set of group X to register RA.

*/
