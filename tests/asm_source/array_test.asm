; =============================================================================
; array_test.asm — Read/write stress test for a 512,000-element integer array
; =============================================================================
; Allocates a 512,000-element INT48 array on the heap.
;
; Write phase:  arr[i] = i * 2   for i = 0 .. 511999
; Read phase:   sum = Σ arr[i]   for i = 0 .. 511999
;
; Expected result:
;   sum = Σ (2*i) for i=0..511999
;       = 2 * (511999 * 512000 / 2)
;       = 511999 * 512000
;       = 262,143,488,000
; =============================================================================

.func main frameSize=0

    ; --- Allocate array ---
    load r0, 512000         ; array length
    load r1, 1              ; element type = TAG_INT48
    arr  r2, r0, r1         ; r2 = pointer to new int array[512000]

    ; --- Write phase: arr[i] = i * 2 ---
    load r3, 0              ; i = 0
    load r4, 1              ; step = 1
    load r5, 512000         ; loop limit

write_loop:
    add    r6, r3, r3       ; r6 = i + i = i * 2
    arrins r6, r2, r3       ; arr[i] = r6
    add    r3, r3, r4       ; i++
    cmp    r3, r5
    jl     @write_loop

    ; --- Read phase: sum = Σ arr[i] ---
    load r7, 0              ; sum = 0
    load r3, 0              ; i = 0

read_loop:
    arrread r8, r2, r3      ; r8 = arr[i]
    add     r7, r7, r8      ; sum += arr[i]
    add     r3, r3, r4      ; i++
    cmp     r3, r5
    jl      @read_loop

    ; --- Output result (expected: 262143488000) ---
    end r7

.endfunc
