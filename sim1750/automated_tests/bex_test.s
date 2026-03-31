; -------------------------------------------------------------
; BEX Address State (AS) Context Switch Test
; -------------------------------------------------------------

        ORG  002AH
BEX_PTR_LP: DATA 0100H         ; Logical Pointer for BEX handler state save
BEX_PTR_SP: DATA 0200H         ; Service Pointer for BEX handler load

        ORG  0100H
LP_SPACE_1: DATA 0000H         ; LP stores MK, SW, IC when trapped
LP_SPACE_2: DATA 0000H
LP_SPACE_3: DATA 0000H

        ORG  0103H
main:
        ; 1. Configure AS=1 Page Registers
        ; Map AS=1 (Instruction & Operand) Logical Page 1 -> Physical Page 1
        ; XIO 51XY WIPR (X=Grp, Y=Page) -> XIO 5111 (Grp 1=AS 1, Page 1)
        LIM  R0, 1          ; Physical Page 1
        XIO  R0, 5111H      ; Write Instruction Page Register
        XIO  R0, 5211H      ; Write Operand Page Register

        ; 2. Initialize our verification flag (R3) to 0
        LIM  R3, 0

        ; 3. Execute BEX 0 to trap into the first handler!
        ; The CPU will save the current MK, SW, IC (pointing to next instruction) to LP (0x0100)
        BEX  0

        ; 4. Execution SHOULD return here after the SECOND BEX (BEX 1)!
        ; Check if R3 == 0x1234. If it is NOT, we fail (hang)
        L    R4, EXPECT_VAL ; Load expected value
        CR   R3, R4         ; Compare R3 against R4
        JC   5, fail_loop   ; Condition 5 = NE (Not Equal) -> hang

        ; Set success flag
        L    R0, SUCCESS_VAL
        LIM  R15, 2000H
        ST   R0, 0, R15

fail_loop:
        BPT

EXPECT_VAL: DATA 1234H
SUCCESS_VAL: DATA -21846    ; 0xAAAA

; -------------------------------------------------------------
; Application Code (AS=1, Physical Page 1)
; -------------------------------------------------------------
        ORG  1050H
app_main:
        ; Execution in AS=1!
        ; Set verification flag in R3 to 0x1234
        L    R3, APP_VAL
        ; Issue BEX 1 to trap back to AS=0 handler 1
        BEX  1

APP_VAL: DATA 1234H

; -------------------------------------------------------------
; BEX Executive Handler (AS=0, Physical Page 0)
; -------------------------------------------------------------
        ORG  0200H
ISR_BEX_MK: DATA 0000H         ; MK for BEX handler
ISR_BEX_SW: DATA 0000H         ; SW for BEX handler (AS=0)
ISR_BEX_IC0: DATA bex_0_handler ; IC for BEX index 0 (SVP + 2 + 0)
ISR_BEX_IC1: DATA bex_1_handler ; IC for BEX index 1 (SVP + 2 + 1)

bex_0_handler:
        ; First pass! App code initialization.
        ; Save the LP state (Original AS=0 main loop state) to 0x0500!
        L    R1, 0100H      ; Load MK
        ST   R1, 0500H
        L    R1, 0101H      ; Load SW
        ST   R1, 0501H
        L    R1, 0102H      ; Load IC
        ST   R1, 0502H

        ; Setup AS=1 State at 0x0400 to jump to Application Code
        LIM  R0, 0000H      ; MK
        ST   R0, 0400H
        ; AS is bits 0-3. AS=1 -> 1000H.
        LIM  R0, 1000H      ; SW: AS=1, PS=0
        ST   R0, 0401H
        LIM  R0, 1050H      ; IC: 1050H (Page 1)
        ST   R0, 0402H

        LST  0400H          ; Load state and jump to AS=1 App Code!

bex_1_handler:
        ; Second pass! BEX 1 handler.
        ; App code executed BEX 1 and we are back in the handler.
        ; We want to return to the original main loop in AS=0!
        ; The CPU saved the App code state to LP (0x0100) on BEX 1.
        ; We don't care about it. We will restore the original state from 0x0500!
        LST  0500H          ; Jump back to main loop!

        END  main
