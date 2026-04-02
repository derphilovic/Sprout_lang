; =============================================================================
; matmul.asm — Triple nested-loop matrix multiply (benchmark)
; =============================================================================
; Simulates C[i][j] = Σ_k A[i][k] * B[k][j]  for N×N matrices
; using synthetic element values:  A[i][k] = i + k,  B[k][j] = k + j
;
; No actual matrix storage (no dynamic indexing in current ISA).
; All values computed on the fly — this exercises the dispatch loop,
; integer arithmetic, and branch prediction, which is the point.
;
; Inner body: 7 instructions × N³ iterations
; Loop overhead: ~5 per middle, ~4 per outer
;
; N = 200  →  8,000,000 inner iterations  →  ~56M instruction dispatches
;
; Expected checksum (exact, int48):
;   Σ_{i,j,k} (i+k)(k+j) = 3·N·S² + N²·S₂
;   where S = N(N-1)/2 = 19900,  S₂ = N(N-1)(2N-1)/6 = 2646700
;   = 3·200·396010000 + 40000·2646700
;   = 237606000000 + 105868000000
;   = 343474000000
;
; To adjust difficulty:
;   N=100 →    7M dispatches,  checksum =  10634250000
;   N=150 →   24M dispatches,  checksum =  81256218750
;   N=200 →   56M disp3tches,  checksum = 343474000000
;   N=300 →  189M dispatches,  checksum = 2616324750000
; =============================================================================

.func main frameSize=0

    ; --- Constants ---
    load r50, 0          ; zero
    load r51, 1          ; increment
    load r52, 300        ; N  (change this to scale benchmark)

    ; --- Accumulators ---
    load r11, 0          ; global checksum (total over all i,j)

    ; --- Outer loop: i = 0 .. N-1 ---
    load r1, 0           ; i = 0

outer:
    ; --- Middle loop: j = 0 .. N-1 ---
    load r2, 0           ; j = 0

middle:
    ; --- Inner loop: k = 0 .. N-1, accumulate dot product ---
    load r10, 0          ; dot product accumulator (reset per i,j pair)
    load r3, 0           ; k = 0

inner:
    ; A[i][k] = i + k,  B[k][j] = k + j
    add  r20, r1, r3     ; r20 = i + k
    add  r21, r3, r2     ; r21 = k + j
    mul  r22, r20, r21   ; r22 = A[i][k] * B[k][j]
    add  r10, r10, r22   ; acc += product

    ; k++
    add  r3, r3, r51
    cmp  r3, r52
    jl   @inner

    ; --- Inner done: fold dot product into checksum ---
    add  r11, r11, r10

    ; j++
    add  r2, r2, r51
    cmp  r2, r52
    jl   @middle

    ; i++
    add  r1, r1, r51
    cmp  r1, r52
    jl   @outer

    ; --- Done ---
    end  r11             ; prints 343474000000  (for N=200)

.endfunc
