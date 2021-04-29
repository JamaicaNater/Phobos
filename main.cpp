#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

#include "Instruction.h"
#include "InstructionTypes.h"

using namespace std;

// Functions
string generateOutputString(string);

// GLOBALS
unordered_map<string, string> R_BINARY_OPCODE_MAP,
                              I_BINARY_OPCODE_MAP,
                              J_BINARY_OPCODE_MAP,
                              BINARY_FUNCTIONCODE_MAP;

string INPUT_FILENAME,
       OUPUT_FILENAME;

int main(int argc, char** argv) {

    if (argc != 3)
    {
        printf("Program requires 2 arguments, received %i", (argc - 1));
        return -1;
    }

    INPUT_FILENAME = argv[1];
    OUPUT_FILENAME = argv[2];


/********************************************************Files*********************************************************/
    ifstream OPCodein,      // OP code in
             BINin,         // Instruction in;
             Funcin;        // Function code in

    ofstream ASMout;    // Assembly out

    OPCodein.open("OPtable.txt");
    if (!OPCodein)
    {
        cout << "Error opening input file (OP codes)";
        return 1;
    }

    BINin.open(INPUT_FILENAME);
    if (!ASMout)
    {
        cout << "Error opening input file (instructions)";
        return 2;
    }

    Funcin.open("FuncTable.txt");
    if(!Funcin)
    {
        cout << "Error opening function table";
        return 3;
    }

    ASMout.open(OUPUT_FILENAME);
    if (!ASMout)
    {
        cout << "Error opening output file";
        return  4;
    }
/********************************************************Files*********************************************************/


    while (!OPCodein.eof())
    {
        char type;
        string opcode,
               operation;

        OPCodein >> type >> opcode >> operation;

        // Lookup Opcode corresponding to binary value
        switch (type)
        {
            case 'R':
                R_BINARY_OPCODE_MAP[opcode] = operation;
                break;

            case 'I':
                I_BINARY_OPCODE_MAP[opcode] = operation;
                break;

            case 'J':
                J_BINARY_OPCODE_MAP[opcode] = operation;
                break;

            default:
                printf("Error symbol '%s' not recognized \n", type);
        }
    }

    while (!Funcin.eof())
    {
        string opcode,
               opname;

        Funcin >> opcode >> opname;

        BINARY_FUNCTIONCODE_MAP[opcode] = opname;
    }


    string binary_instruction;
    while (BINin >> binary_instruction)
    {
        if (binary_instruction.length() != 32)
        {
            cout << "Input code invalid" << endl;
            break;
        }

        ASMout << generateOutputString(binary_instruction);
    }

    return 0;
}

string generateOutputString(string bin_instr)
{
    // Extract the first 6 characters of the line (opcode is first 6 bits)
    string binary_opcode = bin_instr.substr(0, 6),
           opcode,
           output = "Invalid Opcode\n"; // Default output


    // the .find() method returns the .end() iterator if the item is  not found in the list
    // we check if the item is in the list
    if (R_BINARY_OPCODE_MAP.find(binary_opcode) != R_BINARY_OPCODE_MAP.end())
    {
        R_Type assembly_line = R_Type(bin_instr, R_BINARY_OPCODE_MAP[binary_opcode]);

        opcode = assembly_line.opcode;
        string binary_function_code = bin_instr.substr(26, 6);

        if (binary_opcode == "000000")
            opcode = BINARY_FUNCTIONCODE_MAP[binary_function_code];

        // If the shift amount is zero that there is nothing to shift
        if (assembly_line.shamt == "0")
            output = opcode + " " + assembly_line.rd + ", " + assembly_line.rs + ", " + assembly_line.rt + "\n";
        else
            output = opcode + " " + assembly_line.rd + ", " + assembly_line.rs + ", " + assembly_line.shamt
                    + "(" + assembly_line.rt + ")\n";
    }

    if (I_BINARY_OPCODE_MAP.find(binary_opcode) != I_BINARY_OPCODE_MAP.end())
    {
        I_Type assembly_line = I_Type(bin_instr, I_BINARY_OPCODE_MAP[binary_opcode]);

        output = assembly_line.opcode + " " + assembly_line.rs + ", " + assembly_line.rt + ", " + assembly_line.imm
                + "\n";

        if (assembly_line.is_load_store)
            if (atoi(assembly_line.imm.c_str()) > 0)
                output = assembly_line.opcode + " " + assembly_line.rs + ", (" + assembly_line.imm + ")"
                        + assembly_line.rt + "\n";
            else
                output = assembly_line.opcode + " " + assembly_line.rs + ", " + assembly_line.rt + "\n";
    }

    if (J_BINARY_OPCODE_MAP.find(binary_opcode) != J_BINARY_OPCODE_MAP.end())
    {
        J_Type temp = J_Type(bin_instr, J_BINARY_OPCODE_MAP[binary_opcode]);
        output = temp.opcode + " 0x" + temp.imm + "\n";
    }

    return output;
}
