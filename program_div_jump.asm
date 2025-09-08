; Demo DIV + JZ/JNZ + CALL

MOV AX, 20
MOV BX, 5

DIV BX         ; AX = 20 / 5 = 4, DX = 0

CMP AX, 0
JZ zeroCase    ; nếu AX == 0 thì nhảy, nhưng ở đây AX=4 nên bỏ qua

CALL notZero   ; gọi thủ tục khi AX != 0
JMP end

zeroCase:
MOV CX, 111
JMP end

notZero:
DEC AX         ; AX = 3
CMP AX, 3
JZ setCX       ; nếu AX==3 thì CX=222
RET

setCX:
MOV CX, 222
RET

end:
INT 20h        ; kết thúc
