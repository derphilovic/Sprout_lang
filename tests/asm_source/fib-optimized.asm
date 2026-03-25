.func main frameSize=0
load r1, 1
load r32, 56 ;enter fib(targetN - 2) because offset is for some reason fucked
load r33, 1
load r34, 1
load r35, 0
load r30, 0

;r33 = a
;r34 = b
;r35 = temp

fibonacci:
;temp = a
mov r35, r33
;a = a + b
add r33, r33, r34
;print(a)
dbgret r33
;b = temp
mov r34, r35
;increment counter
add r30, r30, r1
cmp r30, r32
jne @fibonacci
end r1

.endfunc
