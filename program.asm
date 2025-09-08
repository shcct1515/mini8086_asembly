; Code demo chương trình

MOV AX, 6
MOV BX, 3

MUL BX        ; AX = 6 * 3 = 18
DIV BX        ; AX = 18 / 3 = 6, DX = 0

CALL subr     ; gọi thủ tục, tăng AX lên 1

CMP AX, 7
JZ equal
MOV CX, 0
JMP end

equal:
MOV CX, 99

end:
INT 20h       ; kết thúc bằng ngắt

;-------------------------
subr:
INC AX        ; AX = AX + 1
RET

