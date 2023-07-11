// Author: Tyler Evans - 661528909

#include <iostream>
#include <fstream>
#include <vector>

using std::cout; using std::cerr;
using std::endl; using std::string;
using std::ifstream; using std::vector;
using std::ios; using std::ofstream;

vector<int> symbol_table;
int pc = 0;

string cond_br_instr[14] = {
    "EQ", "NE", "HS", "LO", "MI", "PL", "VS",
    "VC", "HI", "LS", "GE", "LT", "GT", "LE"
};

string registers[32] = {
    "X0" , "X1" , "X2" , "X3" , "X4" , "X5" , "X6" , "X7" ,
    "X8" , "X9" , "X10", "X11", "X12", "X13", "X14", "X15",
    "X16", "X17", "X18", "X19", "X20", "X21", "X22", "X23",
    "X24", "X25", "X26", "X27", "SP" , "FP" , "LR" , "XZR"
};

string opcodes[27][3] = {
    {"ADD"   , "10001011000", "R" },
    {"ADDI"  , "1001000100" , "I" },
    {"AND"   , "10001010000", "R" },
    {"ANDI"  , "1001001000" , "I" },
    {"B"     , "000101"     , "B" },
    {"B."    , "01010100"   , "CB"},
    {"BL"    , "100101"     , "B" },
    {"BR"    , "11010110000", "R" },
    {"CBNZ"  , "10110101"   , "CB"},
    {"CBZ"   , "10110100"   , "CB"},
    {"EOR"   , "11001010000", "R" },
    {"EORI"  , "1101001000" , "I" },
    {"LDUR"  , "11111000010", "D" },
    {"LSL"   , "11010011011", "R" },
    {"LSR"   , "11010011010", "R" },
    {"ORR"   , "10101010000", "R" },
    {"ORRI"  , "1011001000" , "I" },
    {"STUR"  , "11111000000", "D" },
    {"SUB"   , "11001011000", "R" },
    {"SUBI"  , "1101000100" , "I" },
    {"SUBIS" , "1111000100" , "I" },
    {"SUBS"  , "11101011000", "R" },
    {"MUL"   , "10011011000", "R" },
    {"PRNT"  , "11111111101", "R" },
    {"PRNL"  , "11111111100", "R" },
    {"DUMP"  , "11111111110", "R" },
    {"HALT"  , "11111111111", "R" }
};

/* converts a given input into binary format */
string to_binary(unsigned i) {
    const string DIGITS = "0123456789abcdef";
    string r = i ? to_binary(i / 2) + DIGITS[i % 2] : "00000000";
    if (r.length() > 8) {
        r = r.substr(r.length() - 8);
    }
    return r;
}

/* returns the twos complement of a negative signed binary string */
string find_twos_complement(string str) {
    int n = str.length();

    /* traverse the string to get the first '1' from the back */
    int i;
    for (i = n - 1; i >= 0; i--)
        if (str[i] == '1')
            break;

    /* if no '1' exists than add one to the beginning */
    if (i == -1)
        return '1' + str;

    /* continue traversal after the position of the first '1' */
    for (int k = i - 1; k >= 0; k--) {
        /* just flip the values */
        if (str[k] == '1')
            str[k] = '0';
        else
            str[k] = '1';
    }

    return str;
}

/* simple utility function to convert string representations of binary numbers into base 10 */
string to_decimal_str(string b) {
    int n = stoi(b, 0, 2);
    return std::to_string(n);
}

/* same as above but returns int value instead of string */
int to_decimal(string b) {
    return stoi(b, 0, 2);
}

/* gets the decimal value from a signed binary string */
int to_decimal_signed(string b) {
    if (b[0] == '1') {
        b = find_twos_complement(b);
        return 0 - stoi(b, 0, 2);
    }
    return stoi(b, 0, 2);
}

/* constructs a R-type assembly instruction from binary */
string build_r_instr(string &instr, int opcode_idx) {
    string r = opcodes[opcode_idx][0];                                         // opcode

    /* differentiate between the 5 R-type instruction display formats */
    switch (opcode_idx) {
        case 0:
        case 2:
        case 10:
        case 15:
        case 18:
        case 21:
        case 22:
            /* 'normal' case - opcode followed by 3 registers */
            r.append(" " + registers[to_decimal(instr.substr(27))] + ", ");    // Rd
            r.append(registers[to_decimal(instr.substr(22, 5))] + ", ");       // Rn
            r.append(registers[to_decimal(instr.substr(11, 5))]);              // Rm
            break;
        case 13:
        case 14:
            /* use of shamt */
            r.append(" " + registers[to_decimal(instr.substr(27))] + ", ");    // Rd
            r.append(registers[to_decimal(instr.substr(22, 5))] + ", #");      // Rn
            r.append(to_decimal_str(instr.substr(16, 6)));
            break;
        case 7:
            /* opcode followed by a single register in Rn */
            r.append(" " + registers[to_decimal(instr.substr(22, 5))]);        // Rn
            break;
        case 23:
            /* opcode followed by a single register in Rd */
            r.append(" " + registers[to_decimal(instr.substr(27))]);           // Rd
            break;
        case 24:
        case 25:
        case 26:
            /* just diplay opcode - no registers */
            break;
        default:
            cerr << "Invalid opcode index in this instruction";
            return "Error";
    }

    return r;
}

/* constructs an I-type assembly instruction from binary */
string build_i_instr(string &instr, int opcode_idx) {
    string r = opcodes[opcode_idx][0] + " ";                                   // opcode
    r.append(registers[to_decimal(instr.substr(27))] + ", ");                  // Rd
    r.append(registers[to_decimal(instr.substr(22, 5))] + ", #");              // Rn
    r.append(to_decimal_str(instr.substr(10, 12)));                            // ALU_immediate
    return r;
}

/* constructs a D-type assembly instruction from binary */
string build_d_instr(string &instr, int opcode_idx) {
    string r = opcodes[opcode_idx][0] + " ";                                   // opcode
    r.append(registers[to_decimal(instr.substr(27))] + ", [");                 // Rd
    r.append(registers[to_decimal(instr.substr(22, 5))] + ", #");              // Rn
    r.append(to_decimal_str(instr.substr(11, 9)) + "]");                       // address
    return r;
}

/* constructs a B-type assembly instruction from binary */
string build_b_instr(string &instr, int opcode_idx) {
    string r = opcodes[opcode_idx][0] + " ";                                   // opcode
    /* add the correct label */
    int addr = pc + to_decimal_signed(instr.substr(6));
    int i;
    for (i = 0; i < symbol_table.size(); i++) {
        if (symbol_table.at(i) == addr) {
            r.append("label" + std::to_string(i));
            return r;
        }
    }
    r.append("label" + std::to_string(i));
    symbol_table.push_back(addr);
    return r;
}

/* constructs a CB-type assembly instruction from binary */
string build_cb_instr(string &instr, int opcode_idx) {
    string r = opcodes[opcode_idx][0];                                         // opcode
    switch (opcode_idx) {
        case 8:
        case 9:
            /* opcode followed by Rt then COND_BR_address */
            r.append(" " + registers[to_decimal(instr.substr(27))] + ", ");    // Rt
            break;
        case 5:
            /* opcode with conditional code and then COND_BR_address */
            r.append(cond_br_instr[to_decimal(instr.substr(27))] + " ");       // conditional code
            break;
        default:
            cerr << "Invalid opcode index in this instruction";
            return "Error";
    }
    /* add the correct label */
    int addr = pc + to_decimal_signed(instr.substr(8, 19));
    int i;
    for (i = 0; i < symbol_table.size(); i++) {
        if (symbol_table.at(i) == addr) {
            r.append("label" + std::to_string(i));
            return r;
        }
    }
    r.append("label" + std::to_string(i));
    symbol_table.push_back(addr);
    return r;
}

int main(int argc, char *argv[]) {
    string in_file_name = "";
    
    /* verify input file specified */
    if (argc == 2) {
        in_file_name = argv[1];
    } else {
        cerr << "Usage: ./disasm <LEGv8 binary file>" << endl;
        return EXIT_FAILURE;
    }

    /* verify input file exists */
    ifstream in_file(in_file_name, ios::binary);
    if (!in_file.is_open()) {
        cerr << "Could not open file - '" << in_file_name << "'" << endl;
        return EXIT_FAILURE;
    }

    /* read through the binary file and retrieve the LEGv8 binary instructions */
    vector<string> assembly_instr;
    string instr = "";
    int count = 0;
    while (in_file) {
        instr.append(to_binary((unsigned)in_file.get()));
        count++;
        if (count == 4) {
            assembly_instr.push_back(instr);
            instr.clear();
            count = 0;
        }
    }
    in_file.close();

    /* convert the LEGv8 binary instructions into readable LEGv8 assembly instructions */
    for (int i = 0; i < assembly_instr.size(); i++) {
        /* determine the instruction type */
        char i_type;
        int opcode_idx;
        for (int j = 0; j < 27; j++) {
            if (assembly_instr.at(i).compare(0, opcodes[j][1].length(), opcodes[j][1]) == 0) {
                i_type = opcodes[j][2].at(0);
                opcode_idx = j;
                break;
            }
        }

        /* generate the assembly instruction based on the instruction type */
        switch (i_type) {
            case 'R':
                assembly_instr.at(i) = build_r_instr(assembly_instr.at(i), opcode_idx);
                break;
            case 'I':
                assembly_instr.at(i) = build_i_instr(assembly_instr.at(i), opcode_idx);
                break;
            case 'D':
                assembly_instr.at(i) = build_d_instr(assembly_instr.at(i), opcode_idx);
                break;
            case 'B':
                assembly_instr.at(i) = build_b_instr(assembly_instr.at(i), opcode_idx);
                break;
            case 'C':
                assembly_instr.at(i) = build_cb_instr(assembly_instr.at(i), opcode_idx);
                break;
            default:
                cerr << "Could not determine instruction type for instruction #" << i + 1 << endl;
        }

        /* increment the program counter */
        pc++;
    }

    /* go back through and add labels in */
    for (int i = 0; i < symbol_table.size(); i++) {
        if (symbol_table.at(i) < assembly_instr.size())
            assembly_instr.at(symbol_table.at(i)) = "label" + std::to_string(i) + ": " + assembly_instr.at(symbol_table.at(i));
        else
            assembly_instr.push_back("label" + std::to_string(i) + ": ");
    }

    /* print the assembly instructions */
    for (int i = 0; i < assembly_instr.size(); i++) {
        cout << assembly_instr.at(i) << endl;
    }

    /* uncomment the following code to output assembly code to runnable file */
    // ofstream out_file("disassembled_binary.legv8asm");
    // for (int i = 0; i < assembly_instr.size(); i++) {
    //     out_file << assembly_instr.at(i) << endl;
    // }
    // out_file.close();

    return EXIT_SUCCESS;
}