; =============================================================================
; chudnovsky.asm — Chudnovsky algorithm for pi (10,000 iterations)
; =============================================================================
; π = C / S   where C = 426880 * √10005, S = Σ_{k=0}^{N-1} (M_k * L_k / X_k)
;
; Iterative recurrence (all doubles):
;   w_0 = 1.0  (tracks M_k / X_k)
;   L_0 = 13591409.0
;   S_0 = w_0 * L_0 = 13591409.0
;
;   For k = 1, 2, ..., 9999:
;     ratio_M = (6k-5)(6k-4)(6k-3)(6k-2)(6k-1)(6k) / ((3k-2)(3k-1)(3k) * k^3)
;     w_k = w_{k-1} * ratio_M / (-640320^3)
;     L_k = 13591409 + 545140134 * k
;     S += w_k * L_k
;
;   π = C / S
;
; NOTE: Double precision saturates after ~2 terms. The remaining 9998
; iterations add zero — this is a loop/arithmetic stress test for the VM.
; =============================================================================

.func main frameSize=0

    ; --- Constants ---
    load r10, 1.0
    load r11, 6.0
    load r12, 5.0
    load r13, 4.0
    load r14, 3.0
    load r15, 2.0
    load r16, 545140134.0
    load r17, 13591409.0
    load r18, -262537412640768000.0     ; -(640320^3)
    load r19, 10000.0                   ; iteration limit
    load r20, 42698672.2486239264       ; 426880 * sqrt(10005)

    ; --- Initial term (k = 0) ---
    ; w = 1.0, S = 13591409.0
    load r2, 1.0            ; r2 = w (weight accumulator)
    load r3, 13591409.0     ; r3 = S (sum)
    load r1, 1.0            ; r1 = k (loop counter, starts at 1)

loop:
    ; sixk = 6 * k
    mul  r5, r11, r1        ; r5 = 6k

    ; threek = 3 * k
    mul  r6, r14, r1        ; r6 = 3k

    ; --- Interleaved multiply/divide to keep w in range ---

    ; w *= (6k - 5) / k
    sub  r7, r5, r12        ; r7 = 6k - 5
    mul  r2, r2, r7
    div  r2, r2, r1

    ; w *= (6k - 4) / k
    sub  r7, r5, r13        ; r7 = 6k - 4
    mul  r2, r2, r7
    div  r2, r2, r1

    ; w *= (6k - 3) / k
    sub  r7, r5, r14        ; r7 = 6k - 3
    mul  r2, r2, r7
    div  r2, r2, r1

    ; w *= (6k - 2) / (3k - 2)
    sub  r7, r5, r15        ; r7 = 6k - 2
    sub  r8, r6, r15        ; r8 = 3k - 2
    mul  r2, r2, r7
    div  r2, r2, r8

    ; w *= (6k - 1) / (3k - 1)
    sub  r7, r5, r10        ; r7 = 6k - 1
    sub  r8, r6, r10        ; r8 = 3k - 1
    mul  r2, r2, r7
    div  r2, r2, r8

    ; w *= 6k / 3k
    mul  r2, r2, r5         ; w *= 6k
    div  r2, r2, r6         ; w /= 3k

    ; w /= -640320^3
    div  r2, r2, r18

    ; --- L_k = 13591409 + 545140134 * k ---
    mul  r7, r16, r1
    add  r4, r17, r7        ; r4 = L_k

    ; --- S += w * L_k ---
    mul  r7, r2, r4         ; r7 = term_k
    add  r3, r3, r7         ; S += term_k

    ; --- k++ and loop check ---
    add  r1, r1, r10        ; k += 1
    cmp  r1, r19            ; k < 10000 ?
    jl   @loop

    ; --- pi = C / S ---
    div  r21, r20, r3
    end  r21

.endfunc
