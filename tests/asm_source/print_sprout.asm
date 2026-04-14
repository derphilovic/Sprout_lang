; =============================================================================
; print_sprout.asm — Load a char6 literal and print it
; Expected output: SPROUT
; =============================================================================

.func main frameSize=0

    load r0, "SPROUT"   ; encode 6-char string as NaN-boxed char6
    end  r0             ; prints SPROUT

.endfunc
