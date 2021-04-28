#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cmath>

using namespace std;

class Instruction
{
public:
    string raw_string,
        opcode;

    string binToHex (string str)
    {
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

    I_Type( string instr, string op )
    {
        string bin_opcode = instr.substr(0,6);
        if (bin_opcode == "000100" || bin_opcode == "000101")
            is_branch = true;

        if (bin_opcode == "100100" || bin_opcode == "100000" || bin_opcode == "100101" || bin_opcode == "100001"
            || bin_opcode == "100011" || bin_opcode == "101001" || bin_opcode == "101000" || bin_opcode == "101011")
            is_load_store = true;

        opcode = op;
        rs = '$' + (instr.substr(6, 5));
        rt = '$' + binToDec(instr.substr(11, 5));

        if (is_branch)
            imm =  binToHex(instr.substr(16,16));
        else
            imm = binToDec(instr.substr(16,16));
    }
};

class J_Type: public Instruction
{
public:
    string imm;

    J_Type ( string instr, string op )
    {
        imm =  binToHex(instr.substr(6,26));
        /*
         *  instr.substr(0,26) - take the first 26 characters of the string
         *  that is then passed to the bin to hex method and copied into imm
         */
        opcode = op;
    }
};

string generateOutputString(string);

unordered_map<string, string> Rmap,
        Imap,
        Jmap,
        Funcmap;

int main() {
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

    BINin.open("Binary.txt");
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

    ASMout.open("Assembly.txt");
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
                Rmap[opcode] = operation;
                break;

            case 'I':
                Imap[opcode] = operation;
                break;

            case 'J':
                Jmap[opcode] = operation;
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

        Funcmap[opcode] = opname;
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
    if (Rmap.find(binary_opcode) != Rmap.end())
    {
        R_Type temp = R_Type(bin_instr, Rmap[binary_opcode]);

        opcode = temp.opcode;
        string bin_funct_code = bin_instr.substr(26, 6);

        if (binary_opcode == "000000")
            opcode = Funcmap[bin_funct_code];

        if (temp.shamt == "0")
            output = opcode + " " +  temp.rd + " " + temp.rs + " " + temp.rt + "\n";
        else
            output = opcode + " " + temp.rd + " " + temp.rs + " " + temp.shamt + "(" + temp.rt + ")\n";
    }

    if (Imap.find(binary_opcode) != Imap.end())
    {
        I_Type temp = I_Type(bin_instr, Imap[binary_opcode]);

        output = temp.opcode  + " " + temp.rs + " " + temp.rt + " " + temp.imm + "\n";

        if (temp.is_branch)
            output = temp.opcode  + " " + temp.rs + " " + temp.rt + " 0x" + temp.imm + "\n";

        if (temp.is_load_store)
            if (atoi(temp.imm.c_str()) > 0)
                output = temp.opcode  + " " + temp.rs + " (" +  temp.imm + ")" + temp.rt  + "\n";
            else
                output = temp.opcode  + " " + temp.rs + " " + temp.rt  + "\n";
    }

    if (Jmap.find(binary_opcode) != Jmap.end())
    {
        J_Type temp = J_Type(bin_instr, Jmap[binary_opcode]);
        output = temp.opcode + " " + temp.imm + "\n";
    }

    return output;
}
