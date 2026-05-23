; test08.asm
; Assembly program to show cases where the compiler will throw errors

0badlabel:  ldc 5           ; ERROR: Label starts with a digit
goodlabel:  ldc 10          ; (Valid)
goodlabel:  add             ; ERROR: Duplicate label name
            SET 100         ; ERROR: SET requires a label

            fakeop          ; ERROR: Bogus Mnemonic!
            add 5           ; ERROR: Unexpected operand! (add takes 0)
            ldc             ; ERROR: Missing operand! (ldc takes 1)

            ldc 089         ; ERROR: Not a number (invalid octal)
            ldc 0x1G3       ; ERROR: Not a number (invalid hex)
            ldc 5a          ; ERROR: Not a number (invalid decimal)

            ldc 5, 6        ; ERROR: Extra operand (comma)
            ldl 10 20       ; ERROR: Extra operand (space)

            br nowhere      ; ERROR: No such label (Pass 2)
            call goodlabel  ; (Valid, uses goodlabel so it doesn't warn)

lonely:     HALT            ; WARNING: Unused label