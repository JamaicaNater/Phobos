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
                numerical_val;

            string length_four_binary,
                   to_hex;


            // Gets the first four characters of binary
            if (str.length() < 4)
            {
                substr_len = str.length();

                // if the string we read in is missing 3 character we need 3 zeros in front for it to be length
                for (int i = 4 - substr_len; i > 0; i--)
                    length_four_binary.push_back('0');

                length_four_binary.append(str);
            }

            to_hex = binToDec(length_four_binary)[0];
            numerical_val = atoi(to_hex.c_str());

            if (numerical_val > 9)
            {
                numerical_val -= 10;
                to_hex = 'A';

                while ( numerical_val != 0 )
                {
                    to_hex[0]++;
                    numerical_val--;
                }
            }

            hex_value.append(to_hex);

            str = str.substr(substr_len, str.length() - substr_len);
        }
        return hex_value;

    }

    string binToDec (string str)
    {
        int decimal = 0,
            big_endian_pos;

        for (int i = str.length()-1; i > 0; i--)
        {
            big_endian_pos = str.length() - i - 1;
            char c = str[i];
            //*************** REVIST
            decimal += (c - '0') * pow(2, big_endian_pos);
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

    I_Type( string instr, string op )
    {
 
        imm = binToHex(instr.substr(16,16));
        
        if ( false ) // imm refers to an address
            imm = "0x" + imm;

        opcode = op;
        rs = '$' + binToDec(instr.substr(6, 5));
        rt = '$' + binToDec(instr.substr(11, 5));
    }
};

class J_Type: public Instruction
{
public:
    string imm;

    J_Type ( string instr, string op )
    {
        imm =  "0x" + binToHex(instr.substr(6,26));
        /*
         *  instr.substr(0,26) - take the first 26 characters of the string
         *  that is then passed to the bin to hex method and copied into imm
         */
        opcode = op;
    }
};



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
    unordered_map<string, string> Rmap,
                                  Imap,
                                  Jmap,
                                  Funcmap;
    /*
     *  Loop responsible for filling up the hash table, the loops reads in the
     *  Opcode hash information from a file. Each pair of binary, opcode items
     *  eg (100001, addu) is stored in a hash table corresponding to its R, I, J status.
     */
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

    /*
     *  This is the loop where we iterate through the list of instructions. We get each line which is one instruction,
     *  then  we isolate the first 6 bit to find the binary opcode. Later, we use this value to search the hashmaps
     *  for a match in opcode.
     *
     *  If this is not found then the instrution is not valid.
     */
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

        binary_opcode = binary_instruction.substr(0, 6); // Extract the first 6 characters of the line (opcode is first 6 bits)

        // the .find() method returns the .end() iterator if the item is  not found in the list
        // we check if the item is in the list
        if (Rmap.find(binary_opcode) != Rmap.end())
        {
            R_Type temp = R_Type(binary_instruction, Rmap[binary_opcode]);

            if (binary_opcode != "000000")
                opcode = temp.opcode;
            else
                opcode = Funcmap[binary_instruction.substr(26, 6)];

            if (temp.shamt != "0")
                ASMout << opcode << " " <<  temp.rd << " " << temp.rs << " " << temp.rt << "(" << temp.shamt << ")" << endl;
            else
                ASMout << opcode << " " <<  temp.rd << " " << temp.rs << " " << temp.rt << endl;
        }
        else if (Imap.find(binary_opcode) != Imap.end())
        {
            I_Type temp = I_Type(binary_instruction, Imap[binary_opcode]);
            ASMout << temp.opcode  << " " << temp.rs << " " << temp.rt << " " << temp.imm << endl;
        }
        else if (Jmap.find(binary_opcode) != Jmap.end())
        {
            J_Type temp = J_Type(binary_instruction, Jmap[binary_opcode]);
            ASMout << temp.opcode << " " << temp.imm << endl;
        }
        else
        {
            cout << "Error Invalid";
        }
    }

    return 0;
}
