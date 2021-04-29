using namespace std;

class Instruction
{
public:
    string opcode;

    string binToHex (string);

    string binToDec (string);
};

string Instruction::binToDec(string str)
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

string Instruction::binToHex(string str)
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
#ifndef PHOBOS_INSTRUCTION_H
#define PHOBOS_INSTRUCTION_H

#endif //PHOBOS_INSTRUCTION_H
