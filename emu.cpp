/*
    NAME: SARTHAK PODDAR
    ROLL NO: 2401CS25
    I hereby declare that all the code in this file is an original creative property of mine and has not been plagiarised from anywhere unless specified
*/

#include <bits/stdc++.h>

using namespace std;

int A,B,PC,SP;

vector<int> memory(1e6);

// Load the instructions from the object file in the form of a 32-bit int value into the memory
void loadInstructions(string filename){
    ifstream objfile(filename, ios::binary);
    int instruction;
    int i = 0;

    while(objfile.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) // This reads the memory in little endian format
        memory[i++] = instruction;

    objfile.close();
}

void execution(string filename){
    ofstream traceFile(filename + ".trace");

    traceFile << uppercase;

    long long no_instr = 0;

    while(true){
        
        int curr_pc = PC; // Load the PC and extract the instruction based on its PC from the memory
        int instruction = memory[curr_pc];
        PC++;
        int opcode = instruction & 0xFF; // Mask the the instruction to extract just the last 8 bits which contains the opcode, int opcode will type case it
        int operand = (instruction >> 8); // Right shift by 8 bits to remove the opcode from the instruction and extract the operand value
        string mnemonic = "";

        // Switch case to evaluate the instruction based on the ISA given in the project description
        switch(opcode){
            case 0: B = A; A = operand; mnemonic = "ldc"; break;
            case 1: A = A + operand; mnemonic="adc"; break;
            case 2: B = A; A = memory[SP + operand]; mnemonic="ldl"; break;
            case 3: memory[SP + operand] = A; A = B; mnemonic="stl"; break;
            case 4: A = memory[A + operand]; mnemonic="ldnl"; break;
            case 5: memory[A + operand] = B; mnemonic="stnl"; break;
            case 6: A = B + A; mnemonic="add"; break;
            case 7: A = B - A; mnemonic="sub"; break;
            case 8: A = B << A; mnemonic="shl"; break;
            case 9: A = B >> A; mnemonic="shr"; break;
            case 10: SP = SP + operand; mnemonic="adj"; break;
            case 11: SP = A; A = B; mnemonic="a2sp"; break;
            case 12: B = A; A = SP; mnemonic="sp2a"; break;
            case 13: B = A; A = PC; PC = PC + operand; mnemonic="call"; break;
            case 14: PC = A; A = B; mnemonic="return"; break;
            case 15: if(A == 0) PC = PC + operand; mnemonic="brz"; break;
            case 16: if(A < 0) PC = PC + operand; mnemonic="brlz"; break;
            case 17: PC = PC + operand; mnemonic="br"; break;
            case 18: // Terminate the program when the HALT statement has been read. Returns instead of exiting so that the memory dump may be outputted after the execution is over
                    cout << "The program has been executed. Check .trace for register outputs and .dump for memory dump!" << endl;
                    traceFile << "PC: " << setfill('0') << setw(8) << hex << PC << " A: " << setw(8) << hex << A << " B: " << setw(8) << hex << B << " SP: " << setw(8) << hex << SP << " HALT"<< endl;
                    traceFile.close();
                    return;
        }

        // If the mnemonic do not require a operand then dont print the operand value alongisde the mnemonic name
        if(mnemonic!="add" && mnemonic!="sub" && mnemonic!="shl" && mnemonic!="shr" && mnemonic!="a2sp" && mnemonic!="sp2a" && mnemonic!="return") {
            traceFile << "PC: " << setfill('0') << setw(8) << hex << PC << " A: " << setw(8) << hex << A << " B: " << setw(8) << hex << B << " SP: " << setw(8) << hex << SP << " " << mnemonic << " " << setw(8) << hex << operand << endl;
        }
        else {
            traceFile << "PC: " << setfill('0') << setw(8) << hex << PC << " A: " << setw(8) << hex << A << " B: " << setw(8) << hex << B << " SP: " << setw(8) << hex << SP << " " << mnemonic << endl;
        }

        no_instr++;

        // Checks if the number of instructions executed has exceeded an arbitrary large number and deduces that it must be so because of an infinite loop or due to the absence of a HALT statement
        if(no_instr > 100000) {
            cout << "HALT not reached. Check for infinite loop or HALT instruction in the code" << endl;
            break;
        }
    }
}

void memoryDump(string filename, int when){
    ofstream dumpFile;
    // Memory dump before execution helps in understanding the pointer location of the data values and helps in observing the output of the code
    if(!when){
        dumpFile.open(filename+".dump");
        dumpFile << "----------Memory Dump Before Execution------------\n\n            +0       +1       +2       +3" << endl;
    }
    // Memory dump after execution dumps the entire memory including the instruction memory, the data and the stack memory
    else{
        dumpFile.open(filename+".dump", ios::app);
        dumpFile << "\n\n----------Memory Dump After Execution------------\n\n            +0       +1       +2       +3" << endl;
    }

    dumpFile << uppercase;

    for(int i = 0; i<memory.size();){
        if(memory[i]){
            // The memory address should be in multiples of 4 for better reading and understanding of the memory file
            if(i%4 == 0){
                // Dumps the memory in groups of four in increasing order of memory value
                if((i+3) <= memory.size())
                    dumpFile << setfill('0') << setw(8) << hex << i << " " << setw(8) << hex << memory[i++] << " " << setw(8) << hex << memory[i++] << " " << setw(8) << hex << memory[i++] << " " << setw(8) << hex << memory[i++] << endl;
                else{
                    // If memory size is near exhuastion and less than 4 instructions are yet to be dumped then dump them in order, this prevents any memory out of bounds error due to the previous outputing method
                    int k = memory.size()-i;
                    dumpFile << setfill('0') << setw(8) << hex << i << " ";
                    for(int j = 0; j<k; j++){
                        dumpFile << setw(8) << hex << memory[i++] << " "; 
                    }
                    dumpFile << endl;
                }
            }
            else{
                // If the current memory location has a nono zero value but is not a multiple of 4, start dumping values from the nearest multiple of 4
                i-=(i%4);
                if((i+3) <= memory.size())
                    dumpFile << setfill('0') << setw(8) << hex << i << " " << setw(8) << hex << memory[i++] << " " << setw(8) << hex << memory[i++] << " " << setw(8) << hex << memory[i++] << " " << setw(8) << hex << memory[i++] << endl;
                else{
                    int k = memory.size()-i;
                    dumpFile << setfill('0') << setw(8) << hex << i << " ";
                    for(int j = 0; j<k; j++){
                        dumpFile << setw(8) << hex << memory[i++] << " "; 
                    }
                    dumpFile << endl;
                }
            }
        }
        else{ 
            i++;
        }     
    }

    dumpFile.close();
}

int main(int argc, char* argv[]){
    /* The order of execution is as follows: 
        1. Load the instructions from the object file into the memory
        2. Dump memory contents before the execution of the code to observe the instruction memory
        3. Execute the instructions based on a switch case driven program which works on the principles specified by the ISA of the project
        4. Dump the memory contents after program execution to observe the output and the stack memory
    */
    if(argc != 2)
	{
		cout << "Invalid Program Call: Kindly follow the format ./emu.cpp <filename>.o";
        exit(1);
	}

    string filename = argv[1];
    loadInstructions(filename);
    auto itr = filename.find('.');
    filename = filename.substr(0, itr);
    memoryDump(filename, 0);
    execution(filename);
    memoryDump(filename, 1);

    return 0;
}