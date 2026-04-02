; =============================================================================
; collatz.asm — Collatz sequence step counter
; =============================================================================
; Starting from n = 27 (known to take 111 steps), count steps to reach 1.
;
; Algorithm:
;   while n != 1:
;     if n is even: n = n / 2
;     else:         n = 3n + 1
;     steps++
;
; Even/odd detection without modulo:
;   (n / 2) * 2 == n  →  n is even
;
; No unconditional jump instruction, so we use: cmp rX, rX; je @target
;
; Expected output: 111
; =============================================================================

.func main frameSize=0

    load r0, 670617279         ; n
    load r1, 0          ; steps counter
    load r2, 1          ; constant 1
    load r3, 2          ; constant 2
    load r4, 3          ; constant 3

loop:
    ; if n == 1, we're done
    cmp  r0, r2
    je   @done

    ; steps++
    add  r1, r1, r2

    ; --- even/odd check ---
    div  r5, r0, r3     ; r5 = n / 2  (truncating integer div)
    mul  r6, r5, r3     ; r6 = (n / 2) * 2
    cmp  r6, r0         ; r6 == n  →  even
    je   @is_even

    ; --- ODD: n = 3n + 1 ---
    mul  r0, r0, r4
    add  r0, r0, r2
    ; jmpFlag still != 1 (from the cmp that found odd), so jne fires
    jne  @loop

is_even:
    ; --- EVEN: n = n / 2 ---
    div  r0, r0, r3
    ; unconditional jump back to loop
    cmp  r2, r2         ; force EQ
    je   @loop

done:
    end  r1             ; prints step count (111)

.endfunc
