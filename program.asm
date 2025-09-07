
MOV AX, 5
MOV BX, 3
ADD AX, BX
CMP AX, 8
JZ done
MOV CX, 1
JMP end
done:
MOV CX, 2
end:
HLT
