; Name : Sarthak Poddar
; Roll : 2401CS25
; I hereby declare that all the code in this file is an original creative property of mine and has not been plagiarised from anywhere unless specified
;
; test05.asm
; Add numbers in an array

        ldc 0x1000  ; Assuming 0x1000 to be the start of the stack memory as been done in sample program 4 in the project pdf
        a2sp
        adj -1
        sub         ; to make A = 0
        ldc 0       ; to make A = 0, B = 0
        call main
;
main:   ldc n
        stl 0       ; SP[0] stores the number of instructions to be executed, i.e. SP[0] = n-1
        ldc array
        stl -1      ; SP[1] stores the pointer to the current element, i.e. SP[1] = i
        ldc 0
        stl -2      ; SP[2] stores the sum of the array elements, i.e. SP[2] = sum
;
loop:   ldl 0
        brz end 
        ldl -1      ; A = i
        ldnl 0      ; A = array[i]
        ldl -2      ; B = array[i], A = sum
        add         ; A = array[i] + sum
        stl -2      ; SP[2] stores the sum of the array elements, i.e. SP[2] = sum
        ldl -1      ; A = i
        adc 1       ; A = i + 1
        stl -1      ; SP[1] = i + 1
        ldl 0       ; A = SP[0]
        adc -1      ; A = A - 1
        stl 0       ; SP[0] = SP[0] - 1
        br loop
;
end:    ldl -2      ; A = sum
        ldc array   ; B = sum, A = array[0]
        stnl n      ; array[n] = B
        adj 1
        HALT
;
n:      SET 6
array:  data 3
        data 7
        data 8
        data 10
        data 1
        data 15