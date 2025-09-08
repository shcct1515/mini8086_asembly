; Demo CALL lồng nhau và stack

MOV AX, 2
MOV BX, 5

CALL square      ; gọi thủ tục bình phương AX
CALL multiplyBX  ; gọi thủ tục nhân AX * BX

INT 20h          ; kết thúc

;----------------------------
square:          ; AX = AX * AX
MUL AX           ; ví dụ AX=2 → AX=4
RET

multiplyBX:      ; AX = AX * BX
MUL BX           ; ví dụ AX=4 * 5 = 20
CALL addOne      ; gọi tiếp addOne bên trong
RET

addOne:          ; cộng thêm 1
INC AX           ; AX = 21
RET
