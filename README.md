# 8086 Emulator (Mini Version)

## 📌 Giới thiệu

Đây là một **trình giả lập 8086 đơn giản** viết bằng C++.

* Mục đích: học cách hoạt động cơ bản của Assembly 8086.
* Hỗ trợ một số lệnh cơ bản:

  * `MOV`, `ADD`, `SUB`, `INC`, `DEC`
  * `CMP`, `JMP`, `JZ`, `JNZ`
  * `HLT`
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

## 📝 Ví dụ `program.asm`

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

Khi chạy, AX sẽ = 8 và chương trình nhảy tới nhãn `done`.

---

## ⚠️ Lưu ý quan trọng

* Đây **không phải** code ASM thật sự chạy trên CPU.
* File `.asm` chỉ được **parse và xử lý bằng C++**, không phải binary machine code.
* Tập lệnh chỉ là **một tập con nhỏ** của 8086, không có:

  * RAM, Stack, Segment, Interrupt, I/O.
  * Các lệnh phức tạp như `MUL`, `DIV`, `CALL`, `INT`.
