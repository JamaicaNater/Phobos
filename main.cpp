#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

using namespace std;

// Class definitions
class Instruction
{
public:
        string opcode;

    string binToHex (string str)
    {
        if (str.length() == 16)
            str = "00000100" + str + "00";
        if (str.length() == 26)
            str = "0000" + str + "00";


        string hex_value;
        while (str.length() != 0)
        {
            int substr_len = 4,
                decimal_value;

            string binary_substring,
                   append_to_hex;

            // Gets the first four characters of binary
            if (str.length() < 4)
            {
                substr_len = str.length();

                // if the string we read in is missing 3 character we need 3 zeros in front for it to be length
                for (int i = 4 - substr_len; i > 0; i--)
                    binary_substring.push_back('0');

                binary_substring.append(str);
            }
            else
                binary_substring = str.substr(0, 4);

            append_to_hex = binToDec(binary_substring);
            decimal_value = atoi(append_to_hex.c_str());

            if (decimal_value > 9)
            {
                decimal_value -= 10;
                append_to_hex = 'A';

                while (decimal_value != 0 )
                {
                    append_to_hex[0]++;
                    decimal_value--;
                }
            }

            hex_value.append(append_to_hex);

            str = str.substr(substr_len, str.length() - substr_len);
        }
        return hex_value;

    }

    string binToDec (string str)
    {
        int decimal = 0,
            big_endian_pos = 0;

        for (int i = str.length()-1; i >= 0; i--)
        {
            char c = str[i];
            decimal += (c - '0') * pow(2, big_endian_pos);
            big_endian_pos++;
        }
        return to_string(decimal);
    }
};

class R_Type: public Instruction
{
public:
    string rs,
        rt,
        rd,
        shamt,
        funct;

    R_Type(string instr, string op)
    {
        opcode = op;
        rs = '$' + binToDec(instr.substr(6, 5));
        rt = '$' + binToDec(instr.substr(11, 5));
        rd = '$' + binToDec(instr.substr(16,5));
        shamt = binToDec(instr.substr(21,5));
        funct = binToHex(instr.substr(26, 5));
    }
};

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


    I_Type( string instr, string op )
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
};
unordered_set<string> I_Type::branch_code_set = {"000100", "000101"};
unordered_set<string> I_Type::load_store_code_set = {"100100", "100000", "100101", "100001", "100011", "101001", "101000", "101011"};


class J_Type: public Instruction
{
public:
    string imm;

    J_Type ( string instr, string op )
    {
        imm = binToHex(instr.substr(6,26));
        /*
         *  instr.substr(0,26) - take the first 26 characters of the string
         *  that is then passed to the bin to hex method and copied into imm
         */
        opcode = op;
    }
};

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
        printf("Program requires 2 arguments, recieved %i", (int*)(argc - 1));
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


    string binary_instruction;   // each line of instruction
    while (BINin >> binary_instruction)
    {

        string binary_opcode, // opcode in binary stored here, used as an index for hashmap
               opcode;

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
    string binary_opcode = bin_instr.substr(0, 6), // Extract the first 6 characters of the line (opcode is first 6 bits)
           opcode,
           output = "Invalid Opcode\n";

    // the .find() method returns the .end() iterator if the item is  not found in the list
    // we check if the item is in the list
    if (R_BINARY_OPCODE_MAP.find(binary_opcode) != R_BINARY_OPCODE_MAP.end())
    {
        R_Type temp = R_Type(bin_instr, R_BINARY_OPCODE_MAP[binary_opcode]);

        opcode = temp.opcode;
        string bin_funct_code = bin_instr.substr(26, 6);

        if (binary_opcode == "000000")
            opcode = BINARY_FUNCTIONCODE_MAP[bin_funct_code];

        if (temp.shamt == "0")
            output = opcode + " " +  temp.rd + ", " + temp.rs + ", " + temp.rt + "\n";
        else
            output = opcode + " " + temp.rd + ", " + temp.rs + ", " + temp.shamt + "(" + temp.rt + ")\n";
    }

    if (I_BINARY_OPCODE_MAP.find(binary_opcode) != I_BINARY_OPCODE_MAP.end())
    {
        I_Type temp = I_Type(bin_instr, I_BINARY_OPCODE_MAP[binary_opcode]);

        output = temp.opcode  + " " + temp.rs + ", " + temp.rt + ", " + temp.imm + "\n";

        if (temp.is_load_store)
            if (atoi(temp.imm.c_str()) > 0)
                output = temp.opcode  + " " + temp.rs + ", (" +  temp.imm + ")" + temp.rt  + "\n";
            else
                output = temp.opcode  + " " + temp.rs + ", " + temp.rt  + "\n";
    }

    if (J_BINARY_OPCODE_MAP.find(binary_opcode) != J_BINARY_OPCODE_MAP.end())
    {
        J_Type temp = J_Type(bin_instr, J_BINARY_OPCODE_MAP[binary_opcode]);
        output = temp.opcode + " 0x" + temp.imm + "\n";
    }

    return output;
}
