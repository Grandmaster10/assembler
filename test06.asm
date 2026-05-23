; Name : Sarthak Poddar
; Roll : 2401CS25
; I hereby declare that all the code in this file is an original creative property of mine and has not been plagiarised from anywhere unless specified
;
; test06.asm 
; Program to print first n fibonacci numbers

        ldc 0x1000  ; Assuming 0x1000 to be the start of the stack memory as been done in sample program 4 in the project pdf
        a2sp
        adj -1
        sub         ; to make A = 0
        ldc 0       ; to make A=0 and B=0
        call main
;
main:   ldc arr
        adc 2
        stl -1      ; SP[0] stores ptr to write into memory, i.e. SP[0] = i
        ldc n
        ldnl 0
        adc -2
        stl  0      ; SP[1] stores the number of elements yet to be written into the array, initially SP[0] = n-2 since first two fibonacci numbers till n are 0 and 1
;
loop:   ldl 0       ; A = SP[1]
        brz end
        ldl -1      ; A = i
        adc -1      ; i-1
        ldnl 0      ; A = arr[i-1]
        ldl -1      ; B = arr[i-1], A=i
        adc -2      ; i-2
        ldnl 0      ; A = arr[i-2]
        add         ; A = arr[i-1] + arr[i-2]
        ldl -1      ; B = arr[i-1] + arr[i-2], A=i
        stnl 0      ; arr[i] = B
        adc 1       ; A = i+1
        stl -1      ; SP[0] = i+1
        ldl 0       ; A = n-2
        adc -1      ; A = n-3
        stl 0       ; SP[0] = n-3
        br loop
;
end:    adj 1
        HALT
;
n:      data 15
arr:    data 0
        data 1