; -------------------------------------------------------------
; Timer A and Timer B interrupt test
; -------------------------------------------------------------

        ORG  002EH
MK_PTR_A: DATA 0         ; pointer to MK, SW, IC struct (must be valid memory)
SV_PTR_A: DATA 0500H         ; pointer to Service Pointer (Timer A)

        ORG  0032H
MK_PTR_B: DATA 4         ; Another LP just in case
SV_PTR_B: DATA 0520H         ; pointer to Service Pointer (Timer B)

        ORG  0100H
start:
        ; Set MK (Interrupt Mask) to enable Timer A (bit 7) and Timer B (bit 9)
        LIM  R0, 0140H      ; 0000 0001 0100 0000 = 0x0140
        XIO  R0, 2000H     ; Write to MK

        ; Clear Pending Interrupts (PIR)
        LIM  R0, -1
        XIO  R0, 2001H     ; CLIR

        ; Load Timer A with 0xFF00 (256 ticks until overflow)
        LIM  R0, -256
        XIO  R0, 400AH     ; OTA (Output Timer A)

        ; Load Timer B with 0xFF00
        LIM  R0, -256
        XIO  R0, 400EH     ; OTB (Output Timer B)

        ; Start Timer A
        LIM  R0, 0
        XIO  R0, 4008H     ; TAS (Timer A Start)

        ; Start Timer B
        XIO  R0, 400CH     ; TBS (Timer B Start)

        ; Enable global interrupts
        XIO  R0, 2002H     ; ENBL

        ; Clear R1 (Timer A flag) and R2 (Timer B flag)
        LIM  R1, 0
        LIM  R2, 0

wait_loop:
        ; Check if both flags are 1
        CISP R1, 1     ; Compare R1 to 1 (CISP adds 1 to literal, so 0+1=1)
        JC   5, wait_loop   ; If R1 != 1 (Condition 5 = NE), keep waiting
        CISP R2, 1     ; Compare R2 to 1
        JC   5, wait_loop   ; If R2 != 1, keep waiting

        ; Success! Write 0xAAAA to 0x2000
        LIM  R0, -21846
        LIM  R15, 2000H
        ST   R0, 0, R15     ; Store R0 to address in R15

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
        LIM  R0, 7     ; 7 in right-justified code
        XIO  R0, 2004H     ; RPI (Reset Pending Interrupt)
        XIO  R0, 2002H     ; ENBL (Enable global interrupts)
        LST  0         ; Load state from LP

        ORG  0520H
ISR_B_MK: DATA 0140H         ; MK for Timer B ISR
ISR_B_SW: DATA 0         ; SW for Timer B ISR
ISR_B_IC: DATA isr_timer_B    ; IC for Timer B ISR

isr_timer_B:
        LIM  R2, 1     ; Set flag for Timer B
        ; Clear PIR bit 9
        LIM  R0, 9
        XIO  R0, 2004H
        XIO  R0, 2002H     ; ENBL (Enable global interrupts)
        LST  4         ; Load state from LP

        END  start
