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

            if (line.empty()) continue;

            // label
            if (line.back() == ':') {
                line.pop_back();
                labels[line] = index;
                continue;
            }

            // instr
            istringstream ss(line);
            Instruction inst;
            ss >> inst.op;
            string arg;
            while (getline(ss, arg, ',')) {
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

static int getVal(CPU& cpu, const string& tok) {
    if (tok == "AX" || tok == "BX" || tok == "CX" || tok == "DX")
        return getReg(cpu, tok);
    return stoi(tok);
}

static void printCPU(const CPU& cpu) {
    cout << "AX=" << cpu.AX
        << " BX=" << cpu.BX
        << " CX=" << cpu.CX
        << " DX=" << cpu.DX
        << " IP=" << cpu.IP
        << " Z=" << (cpu.ZF ? 1 : 0) << "\n";
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

    return true;
}

static void runAll(CPU& cpu, Program& prog, string& log) {
    int steps = 0;
    while (step(cpu, prog, log) && steps < 10000) {
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

    cout << "8086 Emulator (header-only)\n";
    cout << "Commands: s=step, a=run all, r=reset, q=quit\n";

    char cmd;
    while (true) {
        printCPU(cpu);
        cout << "intel_8086 > ";
        cin >> cmd;

        if (cmd == 's') {
            if (step(cpu, prog, log)) cout << log;
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
