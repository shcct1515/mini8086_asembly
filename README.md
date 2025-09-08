# 8086 Emulator (Mini Version)

## 📌 Giới thiệu

Đây là một **trình giả lập 8086 đơn giản** viết bằng C++.

* Mục đích: học cách hoạt động cơ bản của Assembly 8086.
* Hỗ trợ một số lệnh cơ bản:

  * `MOV`, `ADD`, `SUB`, `INC`, `DEC`
  * `CMP`, `JMP`, `JZ`, `JNZ`
  * `HLT`
  * **Mới**: `MUL`, `DIV`, `CALL`, `RET`, `INT`

* Có 4 thanh ghi: `AX`, `BX`, `CX`, `DX`, cùng với `IP` (Instruction Pointer) và cờ `Z` (Zero flag).
* **Không phải** CPU 8086 thật, chỉ mô phỏng ý tưởng ASM.

> ⚠️ Nếu bạn cần môi trường sát thực tế hơn, hãy dùng `emu8086` hoặc `NASM + DOSBox`.

---

## 📂 Cấu trúc project

```
/Project
 ├── /emulator
 │     └── emulator.h   # Toàn bộ logic giả lập (header-only)
 ├── program.asm        # File ASM mẫu để test
 ├── program_div_jump.asm        # File ASM ví dụ DIV + JZ/JNZ + CALL để test lại toàn bộ chương trình
 ├── program_nested.asm.asm        # File ASM mẫu để test CALL lồng nhau và stack
 └── README.md

```

---

## 🚀 Cách chạy

1. Clone repo:

   ```bash
   git clone https://github.com/<username>/<repo-name>.git
   ```
2. Build project bằng g++ hoặc bất kì compiler cpp nào
3. Run:

   * Chương trình sẽ in trạng thái CPU.
   * Gõ lệnh:

     * `s` → step (chạy từng lệnh)
     * `a` → run all (chạy hết chương trình)
     * `r` → reset CPU
     * `q` → quit

---

## 📝 Ví dụ chương trình

### 1. Cơ bản (`program.asm`)
```asm
MOV AX, 5
MOV BX, 3
ADD AX, BX
CMP AX, 8
JZ done
MOV CX, 1
done:
HLT
```
👉 Kết quả: AX = 8, nhảy tới nhãn `done`.

---

### 2. Dùng lệnh mới (`program_update.asm`)
```asm
MOV AX, 6
MOV BX, 3
MUL BX        ; AX = 18
DIV BX        ; AX = 6, DX = 0
CALL subr     ; gọi thủ tục tăng AX
CMP AX, 7
JZ equal
MOV CX, 0
JMP end

equal:
MOV CX, 99

end:
INT 20h       ; kết thúc

subr:
INC AX
RET
```
👉 Kết quả: AX = 7, BX = 3, CX = 99, DX = 0

---

### 3. CALL lồng nhau (`program_nested.asm`)
```asm
MOV AX, 2
MOV BX, 5
CALL square      ; AX = 2*2 = 4
CALL multiplyBX  ; AX = 4*5 = 20, sau đó gọi addOne → AX = 21
INT 20h

square:
MUL AX           ; AX = AX * AX
RET

multiplyBX:
MUL BX           ; AX = AX * BX
CALL addOne
RET

addOne:
INC AX
RET
```
👉 Kết quả: AX = 21, BX = 5, CX = 0, DX = 0

---

### 4. DIV + JZ/JNZ + CALL (`program_div_jump.asm`)
```asm
MOV AX, 20
MOV BX, 5
DIV BX         ; AX = 4, DX = 0
CMP AX, 0
JZ zeroCase
CALL notZero
JMP end

zeroCase:
MOV CX, 111
JMP end

notZero:
DEC AX         ; AX = 3
CMP AX, 3
JZ setCX
RET

setCX:
MOV CX, 222
RET

end:
INT 20h
```
👉 Kết quả: AX = 3, BX = 5, CX = 222, DX = 0

---

## ⚠️ Lưu ý quan trọng

* Đây **không phải** code ASM thật sự chạy trên CPU.
* File `.asm` chỉ được **parse và xử lý bằng C++**, không phải binary machine code.
* Tập lệnh chỉ là **một tập con nhỏ** của 8086, chưa có:

  * RAM, Stack thật sự, Segment, Interrupt vector, I/O.
  * ~~Các lệnh phức tạp như `MUL`, `DIV`, `CALL`, `INT`.~~

**Update mới nhất**: Bổ sung lệnh **MUL, DIV, CALL, RET, INT** và thêm nhiều ví dụ minh họa để mở rộng khả năng giả lập.
