; =============================================================================
; factorial.asm — Compute 16! using int48 arithmetic
; =============================================================================
; 16! = 20,922,789,888,000  (fits in int48, max ~140 trillion)
; 17! = 355,687,428,096,000  (overflows int48)
;
; Simple countdown loop:  result *= n; n--; repeat while n > 1
; =============================================================================

.func main frameSize=0

    load r0, 16         ; n = 16
    load r1, 1          ; result = 1
    load r2, 1          ; constant 1 (decrement / compare)

loop:
    mul  r1, r1, r0     ; result *= n
    sub  r0, r0, r2     ; n -= 1
    cmp  r0, r2         ; n > 1 ?
    jg   @loop          ; if n > 1, keep going

    ; n == 1, result holds 16!
    end  r1             ; prints 20922789888000

.endfunc
