#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

//emulator core
struct CPU {
    int AX = 0, BX = 0, CX = 0, DX = 0;
    int IP = 0;
    bool ZF = false; // cờ Zero
    vector<int> stack; // stack để lưu return addresses
};

struct Instruction {
    string op;
    vector<string> args;
};

struct Program {
    vector<Instruction> code;
    map<string, int> labels;

    bool load(const string& filename) {
        ifstream fin(filename);
        if (!fin) return false;
        code.clear(); labels.clear();

        string line;
        int index = 0;
        while (getline(fin, line)) {
            // bỏ comment
            auto pos = line.find(';');
            if (pos != string::npos) line = line.substr(0, pos);

            // trim đơn giản
            line.erase(line.begin(), find_if(line.begin(), line.end(), [](unsigned char ch) { return !isspace(ch); }));
            line.erase(find_if(line.rbegin(), line.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), line.end());

            if (line.empty()) continue;

            // label (có thể có dấu ':' ngay sau tên hoặc sau khoảng trắng)
            if (!line.empty() && line.back() == ':') {
                string lbl = line.substr(0, line.size()-1);
                // trim label
                lbl.erase(lbl.begin(), find_if(lbl.begin(), lbl.end(), [](unsigned char ch) { return !isspace(ch); }));
                lbl.erase(find_if(lbl.rbegin(), lbl.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), lbl.end());
                labels[lbl] = index;
                continue;
            }

            // instr
            istringstream ss(line);
            Instruction inst;
            ss >> inst.op;
            string arg;
            while (getline(ss, arg, ',')) {
                // xóa khoảng trắng trong tham số
                arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());
                if (!arg.empty()) inst.args.push_back(arg);
            }
            code.push_back(inst);
            index++;
        }
        return true;
    }
};

static int& getReg(CPU& cpu, const string& r) {
    if (r == "AX") return cpu.AX;
    if (r == "BX") return cpu.BX;
    if (r == "CX") return cpu.CX;
    return cpu.DX;
}

static int parseNumberMaybeHex(const string& tok) {
    // hỗ trợ dạng 20h hoặc 0x20 hoặc decimal
    if (tok.size() > 1 && (tok.back() == 'h' || tok.back() == 'H')) {
        string s = tok.substr(0, tok.size()-1);
        int val = 0;
        stringstream ss; ss << std::hex << s; ss >> val;
        return val;
    }
    if (tok.size() > 2 && tok[0]=='0' && (tok[1]=='x' || tok[1]=='X')) {
        return stoi(tok, nullptr, 16);
    }
    return stoi(tok);
}

static int getVal(CPU& cpu, const string& tok) {
    if (tok == "AX" || tok == "BX" || tok == "CX" || tok == "DX")
        return getReg(cpu, tok);
    return parseNumberMaybeHex(tok);
}

static void printCPU(const CPU& cpu) {
    cout << "AX=" << cpu.AX
        << " BX=" << cpu.BX
        << " CX=" << cpu.CX
        << " DX=" << cpu.DX
        << " IP=" << cpu.IP
        << " Z=" << (cpu.ZF ? 1 : 0)
        << " SP=" << cpu.stack.size()
        << "\n";
}

static bool step(CPU& cpu, Program& prog, string& log) {
    if (cpu.IP < 0 || cpu.IP >= (int)prog.code.size()) {
        log = "Program ended.\n";
        return false;
    }

    Instruction& ins = prog.code[cpu.IP];
    string op = ins.op;
    transform(op.begin(), op.end(), op.begin(), ::toupper);

    log = op;
    for (auto& a : ins.args) log += " " + a;
    log += "\n";

    // tăng IP trước khi thực thi giống thiết kế ban đầu (để CALL push đúng địa chỉ return)
    cpu.IP++;

    if (op == "MOV") {
        getReg(cpu, ins.args[0]) = getVal(cpu, ins.args[1]);
    }
    else if (op == "ADD") {
        getReg(cpu, ins.args[0]) += getVal(cpu, ins.args[1]);
    }
    else if (op == "SUB") {
        getReg(cpu, ins.args[0]) -= getVal(cpu, ins.args[1]);
    }
    else if (op == "INC") {
        getReg(cpu, ins.args[0])++;
    }
    else if (op == "DEC") {
        getReg(cpu, ins.args[0])--;
    }
    else if (op == "CMP") {
        cpu.ZF = (getVal(cpu, ins.args[0]) == getVal(cpu, ins.args[1]));
    }
    else if (op == "JMP") {
        cpu.IP = prog.labels[ins.args[0]];
    }
    else if (op == "JZ") {
        if (cpu.ZF) cpu.IP = prog.labels[ins.args[0]];
    }
    else if (op == "JNZ") {
        if (!cpu.ZF) cpu.IP = prog.labels[ins.args[0]];
    }
    else if (op == "HLT") {
        log = "HLT reached\n";
        return false;
    }
    // --- các lệnh mới ---
    else if (op == "MUL") {
        // MUL reg  -> AX = AX * reg
        int val = getVal(cpu, ins.args[0]);
        cpu.AX = cpu.AX * val;
        cpu.ZF = (cpu.AX == 0);
    }
    else if (op == "DIV") {
        // DIV reg -> AX = AX / reg ; DX = AX % reg
        int val = getVal(cpu, ins.args[0]);
        if (val == 0) {
            log += "Divide by zero error\n";
            return false;
        }
        int quotient = cpu.AX / val;
        int rem = cpu.AX % val;
        cpu.AX = quotient;
        cpu.DX = rem;
        cpu.ZF = (cpu.AX == 0);
    }
    else if (op == "CALL") {
        // CALL label -> push return IP, jump to label
        cpu.stack.push_back(cpu.IP);
        auto it = prog.labels.find(ins.args[0]);
        if (it == prog.labels.end()) {
            log += "Label not found: " + ins.args[0] + "\n";
            return false;
        }
        cpu.IP = it->second;
    }
    else if (op == "RET") {
        if (cpu.stack.empty()) {
            log += "Stack underflow on RET\n";
            return false;
        }
        cpu.IP = cpu.stack.back();
        cpu.stack.pop_back();
    }
    else if (op == "INT") {
        // INT n : hỗ trợ INT 20h để dừng chương trình
        string arg = ins.args.size() ? ins.args[0] : "0";
        int num = parseNumberMaybeHex(arg);
        if (num == 0x20) {
            log += "INT 20h: program terminated\n";
            return false;
        } else {
            log += "INT "; log += arg; log += " not implemented\n";
        }
    }

    return true;
}

static void runAll(CPU& cpu, Program& prog, string& log) {
    int steps = 0;
    while (step(cpu, prog, log) && steps < 100000) {
        cout << log;
        steps++;
    }
}

//public API
inline void runEmulator(const string& filename) {
    Program prog;
    if (!prog.load(filename)) {
        cerr << "Khong mo duoc file: " << filename << "\n";
        return;
    }

    CPU cpu;
    string log;

    cout << "8086 Emulator (header-only) - with MUL/DIV/CALL/RET/INT\n";
    cout << "Commands: s=step, a=run all, r=reset, q=quit\n";

    char cmd;
    while (true) {
        printCPU(cpu);
        cout << "intel_8086 > ";
        cin >> cmd;

        if (cmd == 's') {
            if (step(cpu, prog, log)) cout << log;
            else cout << log;
        }
        else if (cmd == 'a') {
            runAll(cpu, prog, log);
        }
        else if (cmd == 'r') {
            cpu = CPU();
            log.clear();
            cout << "CPU reset!\n";
        }
        else if (cmd == 'q') {
            break;
        }
    }
}
