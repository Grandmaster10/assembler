/*
    NAME: SARTHAK PODDAR
    ROLL NO: 2401CS25
    I hereby declare that all the code in this file is an original creative property of mine and has not been plagiarised from anywhere unless specified
*/

#include <bits/stdc++.h>

using namespace std;

// To store the various components of a SIMPLEX assembly line along with the program counter
struct line_components {
    int pc;
    string label;
    string mnemonic;
    string op;
    string comment;
};

// Global maps and vectors
map<string, pair<string, int>> instruction; // map<mnemonic, pair<opcode, type>> -> types: 0 - no operand, 1 - value, 2 - offset, 3 - data & SET
map<string, pair<int, int>> labels; // map<label_name, pair<pc/value, set_label>> -> pc stores SET value in case set_label = 1
vector<line_components> line;
vector<pair<int, string>> errors; // vector<line_no, error> 
vector<string> warnings;
vector<string> used_labels;
vector<string> og_line;

// Utility Functions
void instructionDetail();
string &trim(string&);
bool isValidNumber(string);
bool isValidInstruction(string);
int typeOfNumber(string);
string octtodec(string);
string hextodec(string);
string dectohex(string);
void seperateIntoComponents(string, int);
void instructionErrors(string, string, int);
void labelErrors(string, int);
void firstPass(string);
string machine_code(line_components, int);
void secondPass(string);
void writeErrors(string);

// Fills the instruction table with mnemonic name and expected operands
void instructionDetail(){
    instruction.insert({"ldc",make_pair("00",1)});
    instruction.insert({"adc",make_pair("01",1)});
    instruction.insert({"ldl",make_pair("02",2)});
    instruction.insert({"stl",make_pair("03",2)});
    instruction.insert({"ldnl",make_pair("04",2)});
    instruction.insert({"stnl",make_pair("05",2)});
    instruction.insert({"add",make_pair("06",0)});
    instruction.insert({"sub",make_pair("07",0)});
    instruction.insert({"shl",make_pair("08",0)});
    instruction.insert({"shr",make_pair("09",0)});
    instruction.insert({"adj",make_pair("0A",1)});
    instruction.insert({"a2sp",make_pair("0B",0)});
    instruction.insert({"sp2a",make_pair("0C",0)});
    instruction.insert({"call",make_pair("0D",2)});
    instruction.insert({"return",make_pair("0E",0)});
    instruction.insert({"brz",make_pair("0F",2)});
    instruction.insert({"brlz",make_pair("10",2)});
    instruction.insert({"br",make_pair("11",2)});
    instruction.insert({"HALT",make_pair("12",0)});
    instruction.insert({"SET", make_pair("",3)});
    instruction.insert({"data", make_pair("",3)});
}

// To trim the whitespaces at the start and end of a string
string &trim(string &s){
    s.erase(find_if(s.rbegin(), s.rend(),not_fn(static_cast<int(*)(int)>(isspace))).base(), s.end());
    s.erase(s.begin(), find_if(s.begin(), s.end(), not_fn(static_cast<int(*)(int)>(isspace))));
    return s;   
}

// Checks symbol by symbol whether the operand is a valid number, acceps numbers with a +/- at the start and which are decimal, hexadecimal or octal
bool isValidNumber(string num){
    regex validation(R"(^[-+]?((0[0-7]*)|(0[xX][0-9a-fA-F]+)|([1-9][0-9]*))$)");
    return regex_match(num, validation);
}

// To check if the mnemonic is in the instruction table or not
bool isValidInstruction(string instr){
    if (instruction.find(instr) != instruction.end())
        return true;
    else return false;
}

// Classifies numbers into decimal, hexadecimal and octal -> 0: decimal, 1: hexadecimal, 2: octal
int typeOfNUmber(string num){
    if(num[0] != '0' || num.length() == 1){
        return 0;
    }
    else {
        if(num[1] != 'x' && num[1] != 'X')
            return 1;
        else
            return 2;
    }
}

// Utility function to convert octal to decimal
string octtodec(string oct){
    string dec = to_string(stoi(oct, nullptr, 8));
    return dec;
}

// Utility function to convert hexadecimal to decimal
string hextodec(string hex){
    string dec = to_string(stoul(hex, nullptr, 16));
    return dec;
}

// Utility function to convert decimal operand to hexadecimal operand
string dectohex(string dec){
    uint32_t decl;
    decl = static_cast<uint32_t>(stoll(dec, nullptr, 0)) & 0xFFFFFF; // converts string dec to base 16 and masks it to 24 bits
    stringstream ss;
    ss << setfill('0') << setw(6) << hex << decl;
    return ss.str();
}

// Seperate a given line into its components -> label, mnemonic, operand, comment alongside the pc
void seperateIntoComponents(string curr_line,  int pc){
    string label = "", mnemonic = "", op = "", comm = "";
    curr_line=trim(curr_line);

    // Push empty lines into the table
    if(curr_line.empty()){
        line_components l = {pc, label, mnemonic, op, comm};
        line.push_back(l);
        return;
    };

    // Extract comment from the line and remove it from the curr_line string for further processing
    auto itr = curr_line.find(';');
    if (itr != string::npos) {
        comm = curr_line.substr(itr + 1);
        comm = trim(comm);
        curr_line = curr_line.substr(0, itr);
    }

    // Extract the label from the line and remove it from the curr_line for further processing
    auto jtr = curr_line.find(':');
    if (jtr != string::npos) {
        label = curr_line.substr(0, jtr);
        label = trim(label);
        curr_line = curr_line.substr(jtr + 1);
    }
    curr_line = trim(curr_line);


    if (!curr_line.empty()) {

        // Extract the mnemonic and the operand from the line given they're space seperated. If the mnemonic doesnt require an operand then just extract the mnemonic
        auto ktr = curr_line.find(' ');
        if (ktr != string::npos) {
            mnemonic = curr_line.substr(0, ktr);
            mnemonic = trim(mnemonic);
            op = curr_line.substr(ktr + 1);
            op = trim(op);
            if(!op.empty() && op[0] == '+'){
                op = op.substr(1, op.length()-1);
            }
        } 
        else mnemonic = curr_line; 
    }

    line_components l = {pc, label, mnemonic, op, comm}; // Construct a struct of all the extracted portions and push it onto the lines table
    line.push_back(l);
}

void instructionErrors(string mnemonic, string op, int ln_no){
    if(!instruction.count(mnemonic)){
        errors.push_back(make_pair(ln_no, "Bogus Mnemonic!"));
        return;
    }

    // mnemonics of type 0 dont require operands and any instance of it will throw an error
    if(instruction[mnemonic].second == 0 && op!=""){
        errors.push_back(make_pair(ln_no, "Unexpected operand!"));
        return;
    }

    // Assumes that a 2nd operand (not supported by the ISA) is comma or space separated  and throws an error
    if(op.find(' ') != string::npos || op.find(',') != string::npos){
        errors.push_back(make_pair(ln_no, "Extra operand on the end of line"));
        return;
    }

    // mnemonics of type > 0 expect a mnemonic and any absence of such will throw an error
    if(instruction[mnemonic].second > 0 && op==""){
        errors.push_back(make_pair(ln_no, "Missing operand!"));
        return;
    }

    //If the mnemonic has an operand and its first character is a digit or a minus but its not a valid number, throw an appropriate return
    if(!op.empty() && (isdigit(op[0]) || op[0]=='-')){
        if(!isValidNumber(op)){
            errors.push_back(make_pair(ln_no, "Not a number"));
            return;
        }
    }
}

void labelErrors(string label, int ln_no){

    // If the label already exists in the label table throw an error
    if(labels.count(label)){
        errors.push_back(make_pair(ln_no, "Duplicate label name"));
    }

    // If the label starts with a digit throw an error
    if(isdigit(label[0])){
        errors.push_back(make_pair(ln_no, "Bogus Label name"));
    }
}

// Reads the program into an internal form (line_components), extracts the labels and fills them into a symbol table and processes common errors( except no such label)
void firstPass(string filename){
    string curr_line;
    int pc = 0;
    int ln_no = 1;
    ifstream inFile(filename);

    while(getline(inFile, curr_line)){

        // If the line is empty push it into the lines table nonetheless
        if(curr_line.empty()){
            og_line.push_back(curr_line);
            seperateIntoComponents(curr_line, pc);
            ln_no++;
            continue;
        }

        og_line.push_back(curr_line);
        seperateIntoComponents(curr_line,pc);

        // If the line has a label
        if(!line[ln_no-1].label.empty()) {
            labelErrors(line[ln_no-1].label, ln_no);
            // If the label mnemonic is empty or its not a SET instruction, simply push its PC and set_label = 0
            if(line[ln_no-1].mnemonic.empty() || line[ln_no-1].mnemonic!="SET") 
                labels.insert({line[ln_no-1].label, make_pair(pc, 0)});
            else {
                // If the SET instruction has a number, convert it to its corresponding HEX and store it in place of the labels pc
                if(!line[ln_no-1].op.empty() && isValidNumber(line[ln_no-1].op)){
                    string temp = ""; int offset = 0;
                    if(typeOfNUmber(line[ln_no-1].op) == 1){
                        temp=octtodec(line[ln_no-1].op); 
                        offset = stoi(temp);
                    }
                    else if(typeOfNUmber(line[ln_no-1].op) == 2){
                        temp=hextodec(line[ln_no-1].op); 
                        offset = stoi(temp);
                    }
                    else{
                        offset = stoi(line[ln_no-1].op);
                    }
                    labels.insert({line[ln_no-1].label, make_pair(offset, 1)});
                }
            }
        }

        // SET instruction requires a label and absence of it should throw an error
        if(line[ln_no-1].label.empty() && line[ln_no-1].mnemonic == "SET"){
            errors.push_back(make_pair(ln_no, "SET instruction requires a valid label in the same line"));
        }

        // Check for errors related to mnemonics and operands
        if(!line[ln_no-1].mnemonic.empty()) instructionErrors(line[ln_no-1].mnemonic, line[ln_no-1].op, ln_no);

        // SET instruction does not increase the PC all other instructions do
        if(line[ln_no-1].mnemonic!="" && line[ln_no-1].mnemonic!="SET"){
            pc++;
        }
        ln_no++;
    }
    inFile.close();
}

string machine_code(line_components line, int ln_no){
    // SET instruction doesnt have a corresponding machine code
    if(line.mnemonic.empty() || (line.mnemonic == "SET")){
        return "        ";
    }

    if(line.mnemonic=="data"){
        uint32_t decl;
        decl = static_cast<uint32_t>(stoll(line.op, nullptr, 0)) & 0xFFFFFFFF; // Utilise all 32 bits of the machine code to store the data value
        stringstream ss;
        ss << setfill('0') << setw(8) << hex << decl;
        return ss.str();
    }

    // If the instruction doesnt have an operand keep the operand 0 and append the opcode at the end
    if(line.op.empty()){
        string result = "000000";
        string opcode = instruction[line.mnemonic].first;
        result = result + opcode;
        return result;
    }

    if(!line.op.empty()){
        // Convert the operand to a 24 bit hex from its initial type and append it to the front of the 8 bit opcode forming a 8 bit machine code
        if(isValidNumber(line.op)){
            string result;
            if(typeOfNUmber(line.op) == 1){
                string dec = octtodec(line.op);
                result = dectohex(dec);
                string opcode = instruction[line.mnemonic].first;
                result = result + opcode;
            }
            else if(typeOfNUmber(line.op) == 2){
                string dec = hextodec(line.op);
                result = dectohex(dec);
                string opcode = instruction[line.mnemonic].first;
                result = result + opcode;
            }
            else{
                result = dectohex(line.op);
                string opcode = instruction[line.mnemonic].first;
                result = result + opcode;
            }
            return result;
        }
        else{
            // Dont calculate machine code for erroneous label operands
            if(isdigit(line.op[0]) || line.op[0] == '-' || line.op[0] == '+' || line.op.find(' ') != string::npos || line.op.find(',') != string::npos) {
                return "";
            }

            // Check if a label in an operand actually exists
            if(!labels.count(line.op)){
                errors.push_back(make_pair(ln_no, "No such label"));
                return "";
            }
            else{
                used_labels.push_back(line.op);
                // Operand value is simply the pc value stored in the symbol table for non branching and call instructions
                string result = dectohex(to_string(labels[line.op].first));
                string opcode = instruction[line.mnemonic].first;
                // If the mnemonic is of branch type or is a call instruction, resulting operand value should be calculated using PC relative addressing where operand = dest_pc - (curr_pc + 1)
                if(labels[line.op].second == 0 && (line.mnemonic == "br" || line.mnemonic == "brlz" || line.mnemonic == "brz" || line.mnemonic == "call")){
                    result = dectohex(to_string(labels[line.op].first - (line.pc + 1)));
                }
                result = result + opcode;
                return result;
            }
        }
    }
    return "";
}

void secondPass(string filename){
    vector<string> lst;
    vector<uint32_t> obj;

    int ln_no = 1;

    // Second Pass uses the internal form for processing since this method has less file IO and time is a stricter constraint than memory
    for(line_components& l : line){
        string mac_code = machine_code(l, ln_no);
        transform(mac_code.begin(), mac_code.end(), mac_code.begin(), ::toupper); // convert machine code to capital
        uint32_t decl;
        decl = static_cast<uint32_t>(stoll(to_string(l.pc), nullptr, 0)) & 0xFFFFFFFF; // convert pc into 8 bit hex for lst file and make it uppercase
        stringstream ss;
        ss << setfill('0') << setw(8) << hex << decl;
        string pc = ss.str();
        transform(pc.begin(), pc.end(), pc.begin(), ::toupper);
        string curr_line = "";
        curr_line = curr_line + l.label;
        string lst_line = pc + " " + mac_code + " " + og_line[ln_no++-1]; // Print the PC, machine code and the original corresponding line from the asm source file into the LST file
        lst.push_back(lst_line);
        // Push non blank machine codes into the objFile
        if(mac_code!="" && mac_code!="        "){
            uint32_t dec = stoul(mac_code, nullptr, 16);
            obj.push_back(dec);
        }
    }

    // Only Generate the object and listing files if the program has compiled without errors
    if(errors.empty()){
        ofstream objfile;
        ofstream lstfile;

        // A true binary file
        objfile.open(filename+".o", ios::binary);
        lstfile.open(filename+".lst");

        for(auto& obj_line : obj){
           objfile.write(reinterpret_cast<const char*>(&obj_line), sizeof(obj_line)); // Convert the obj lines to binary
        }

        for(auto& lst_line : lst){
            lstfile << lst_line << endl;
        }
        
        objfile.close();
        lstfile.close();
    }
}

// Print warnings followed by errors into a log file. Errors are accompanied by the line numbers
void writeErrors(string filename){
    ofstream logfile(filename+".log");
    sort(errors.begin(), errors.end());
    for(auto i : labels){
        string str = i.first;
        auto itr = find(used_labels.begin(), used_labels.end(), str);
        if(itr == used_labels.end()){
            string warning_msg = "WARNING: Unused label ";
            warning_msg = warning_msg + str;
            warnings.push_back(warning_msg);
        }
    }

    for(auto &warning : warnings){
        logfile << warning << endl;
    }

    for(auto &error : errors){
        logfile << "ERROR AT LINE " << error.first << ": " << error.second << endl;
    }
    logfile.close();
}

int main(int argc, char* argv[]){
    /* The order of execution is as follows: 
        1. Fill the Instruction Table
        2. Perform the first pass to fill the symbol table and catch common errors
        3. Perform the 2nd pass to convert parsed lines to machin, catch more erros and write the outputs into a object and listing file
        4. Write the warnings and errors generated by the program. 
    */
    instructionDetail();
    if(argc != 2)
	{
		cout << "Invalid Program Call: Kindly follow the format ./asm.cpp <filename>.asm";
        exit(1);
	}

    string filename = argv[1];
    firstPass(filename);
    auto itr = filename.find('.');
    filename = filename.substr(0, itr);
    secondPass(filename);
    writeErrors(filename);
    if(errors.size()){
        cout << "The program has been compiled with errors, check the log file for errors. Object file and Listing file have not been generated." << endl;
    }
    else{
        cout << "The program has compiled successfully!! The listing file (.lst) and the object file (.o) have been successfully generated. You may observe the output using the emulator and the object file." << endl;
    }

    return 0;
}