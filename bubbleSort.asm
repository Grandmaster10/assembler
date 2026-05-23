; Name : Sarthak Poddar
; Roll : 2401CS25
; I hereby declare that all the code in this file is an original creative property of mine and has not been plagiarised from anywhere unless specified
;
; test07.asm
; Sort a given array using Bubble Sort
;
        ldc 0x1000  ; Assuming 0x1000 to be the start of the stack memory as been done in sample program 4 in the project pdf
        a2sp
        adj -1
        sub         ; to make A = 0
        ldc 0       ; to make A = 0, B = 0
        call main
;
main:   ldc arr
        stl -1      ; SP[1] stores the ptr to the current array element for the main loop, i.e. SP[1] = i
        ldc n
        ldnl 0
        ; adc -1   
        stl 0       ; SP[0] stores the number of instructions left to evaluate for the main loop, i.e. SP[0] = n-1
;
loop:   ldl 0
        brz end
        ldc arr
        stl -3      ; SP[3] stores the ptr to the current array element for the sub loop, i.e. SP[3] = j
        ldc n
        ldnl 0
        adc -1   
        stl -2      ; SP[2] stores the number of instructions left to evaluate for the sub loop, i.e. SP[2] = n-1
sloop:  ldl -2
        brz subend
        ldl -1      ; A = i   
        ldnl 0      ; A = arr[i]
        ldl -3      ; B = arr[i], A = j
        ldnl 0      ; A = arr[j]
        sub         ; A = arr[i] - arr[j]
        brlz swap
        ldl -3      ; A = j
        adc 1       ; j = j + 1
        stl -3      ; SP[3] = j+1
        ldl -2      ; A = SP[2]
        adc -1      ; A = SP[2] - 1
        stl -2      ; SP[2] = SP[2] - 1
        br sloop
;
swap:   ldl -1      ; A = i
        ldnl 0      ; A = arr[i]
        stl -4      ; SP[4] stores the temporary value used for swapping, i.e. SP[4] = temp
        ldl -3      ; B = arr[i], A = j
        ldnl 0      ; A = arr[j]
        ldl -1      ; B = arr[j], A = i
        stnl 0      ; arr[i] = arr[j]
        ldl -4      ; A = temp
        ldl -3      ; B = temp, A = j
        stnl 0      ; arr[j] = temp
        ldl -3      ; A = j
        adc 1       ; j = j + 1
        stl -3      ; SP[3] = j + 1
        ldl -2      ; A = SP[2]
        adc -1      ; A = SP[2] - 1
        stl -2      ; SP[2] = SP[2] - 1
        br sloop
;
subend: ldl -1      ; A = i
        adc 1       ; i = i + 1
        stl -1      ; SP[1] = i + 1
        ldl 0       ; A = SP[0]
        adc -1      ; A = SP[0] - 1
        stl 0       ; SP[0] = SP[0] - 1
        br loop
;
end:    adj 1
        HALT
;
n:      data 10
arr:    data -5
        data 10
        data 3
        data 15
        data -15
        data 6
        data -10
        data 8
        data -3
        data 7
