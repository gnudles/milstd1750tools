; -------------------------------------------------------------
; Timer A and Timer B interrupt test
; -------------------------------------------------------------

        ORG  002EH
MK_PTR_A: DATA 0300H        ; pointer to MK, SW, IC struct (must be valid memory)
SV_PTR_A: DATA 0500H         ; pointer to Service Pointer (Timer A)

        ORG  0032H
MK_PTR_B: DATA 0304H         ; Another LP just in case
SV_PTR_B: DATA 0520H         ; pointer to Service Pointer (Timer B)

        ORG  0100H
start:
        ; Set MK (Interrupt Mask) to enable Timer A (bit 7) and Timer B (bit 9)
        LIM  R0, 0140H      ; 0000 0001 0100 0000 = 0x0140
        XIO  R0, 2000H     ; Write to MK

        ; Clear Pending Interrupts (PIR)
        LIM  R0, -1
        XIO  R0, 2001H     ; CLIR

        ; Load Timer A
        LIM  R0, -64
        XIO  R0, 400AH     ; OTA (Output Timer A)

        ; Load Timer B
        LIM  R0, -4
        XIO  R0, 400EH     ; OTB (Output Timer B)

        ; Start Timer A
        LIM  R0, 0
        XIO  R0, 4008H     ; TAS (Timer A Start)

        ; Start Timer B
        XIO  R0, 400CH     ; TBS (Timer B Start)



        ; Clear R1 (Timer A flag) and R2 (Timer B flag)
        LIM  R1, 0
        LIM  R2, 0
        ; Enable global interrupts
        XIO  R0, 2002H     ; ENBL

wait_loop1:
        ; Check if both flags are 1
        CISP R1, 1     ; Compare R1 to 1 (CISP adds 1 to literal, so 0+1=1)
        BNZ    wait_loop1   ; If R1 != 1, keep waiting
wait_loop2:
        CISP R2, 1     ; Compare R2 to 1
        BNZ    wait_loop2   ; If R2 != 1, keep waiting

        ; Success! Write 0xAAAA to 0x2000
	LIM	R0, 0xAAAA
	ST	R0, 0x2000

        ; Halt
        BPT

; -------------------------------------------------------------
; Interrupt Service Routines
; -------------------------------------------------------------
        ORG  0500H
ISR_A_MK: DATA 0140H         ; MK for Timer A ISR
ISR_A_SW: DATA 0         ; SW for Timer A ISR
ISR_A_IC: DATA isr_timer_A    ; IC for Timer A ISR

isr_timer_A:
        LIM  R1, 1     ; Set flag for Timer A
        ; Clear PIR bit 7
        XIO  R0, 2002H     ; restore interrupts
        LST 0300H         ; Load state from LP

        ORG  0520H
ISR_B_MK: DATA 0140H         ; MK for Timer B ISR
ISR_B_SW: DATA 0         ; SW for Timer B ISR
ISR_B_IC: DATA isr_timer_B    ; IC for Timer B ISR

isr_timer_B:
        LIM  R2, 1     ; Set flag for Timer B
        XIO  R0, 2002H     ; restore interrupts
        LST 0304H         ; Load state from LP

        END  start
