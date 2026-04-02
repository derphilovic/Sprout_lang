; =============================================================================
; euler.asm — Compute Euler's number e via Taylor series (doubles)
; =============================================================================
; e = Σ_{k=0}^{N} 1/k!  =  1 + 1 + 1/2 + 1/6 + 1/24 + ...
;
; Iterative: term_k = term_{k-1} / k
; Converges fully in ~18 terms for double precision.
; We run 25 iterations to be safe.
;
; Expected output: 2.71828182845904...  (IEEE 754 double precision)
; =============================================================================

.func main frameSize=0

    load r0, 1.0        ; term = 1/0! = 1.0
    load r1, 1.0        ; sum = 1.0  (first term)
    load r2, 1.0        ; k = 1.0
    load r3, 1.0        ; constant 1.0 (increment)
    load r4, 25.0       ; iteration limit

loop:
    ; term = term / k
    div  r0, r0, r2     ; term /= k

    ; sum += term
    add  r1, r1, r0

    ; k += 1
    add  r2, r2, r3

    ; if k <= 25, continue
    cmp  r2, r4
    jl   @loop
    ; also do k == 25
    je   @loop

    end  r1             ; prints e ≈ 2.71828182845905

.endfunc
