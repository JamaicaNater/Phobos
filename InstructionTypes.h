
using namespace std;

class R_Type: public Instruction
{
public:
    string rs,
            rt,
            rd,
            shamt,
            funct;

    R_Type(string, string);
};
R_Type::R_Type(string instr, string op)
{
    opcode = op;
    rs = '$' + binToDec(instr.substr(6, 5));
    rt = '$' + binToDec(instr.substr(11, 5));
    rd = '$' + binToDec(instr.substr(16,5));
    shamt = binToDec(instr.substr(21,5));
    funct = binToHex(instr.substr(26, 5));
}


class I_Type: public Instruction
{
public:
    string rs,
            rt,
            imm;

    bool is_branch = false,
            is_load_store = false;

    static unordered_set<string> branch_code_set,
            load_store_code_set;


    I_Type( string , string );
};
unordered_set<string> I_Type::branch_code_set = {"000100", "000101"};
unordered_set<string> I_Type::load_store_code_set = {"100100", "100000", "100101", "100001", "100011", "101001", "101000", "101011"};

I_Type::I_Type( string instr, string op )
{
    string bin_opcode = instr.substr(0,6);

    opcode = op;
    rs = '$' + binToDec(instr.substr(6, 5));
    rt = '$' + binToDec(instr.substr(11, 5));
    imm = binToDec(instr.substr(16,16));

    if (branch_code_set.find(bin_opcode) != branch_code_set.end() )
        is_branch = true;

    if (load_store_code_set.find(bin_opcode) != load_store_code_set.end() )
        is_load_store = true;
}


class J_Type: public Instruction
{
public:
    string imm;

    J_Type ( string, string );
};
J_Type::J_Type ( string instr, string op )
{
    imm = binToHex(instr.substr(6,26));
    opcode = op;
}

#ifndef PHOBOS_INSTRUCTIONTYPES_H
#define PHOBOS_INSTRUCTIONTYPES_H

#endif //PHOBOS_INSTRUCTIONTYPES_H
