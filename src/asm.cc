#include "kind.h"
#include "lexer.h"
#include "assembler.h"
#include "utils.h"
#include <vector>
#include <string>
#include <iostream>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cin;
using std::cout;
using std::getline;
using ASM::Token;
using ASM::Lexer;
using ASM::Assembler;
using ASM::Utils;

int main(int argc, char* argv[]){
    // Nested vector representing lines of Tokens
    // Needs to be used here to cleanup in the case
    // of an exception
    vector< vector<Token*> > tokLines;
    Assembler mips;

    try{
        // Create a MIPS recognizer to tokenize
        // the input lines
        Lexer lexer;
        // Tokenize each line of the input
        string line;

        while(getline(cin,line)){
            if (line.empty()) continue;
            vector<Token*> inst = lexer.scan(line);
            tokLines.push_back(inst);
            mips.Pass1(inst);
        }

        vector< vector<Token*> >::iterator i;
        for (i = tokLines.begin(); i != tokLines.end(); i++) {
            mips.Pass2(*i);
        }

        mips.printBinaryLines();
        mips.printSymTable();

/*      // Iterate over the lines of tokens and print them
        // to standard error
        vector<vector<Token*> >::iterator it;
        for(it = tokLines.begin(); it != tokLines.end(); ++it){
            vector<Token*>::iterator it2;
            for(it2 = it->begin(); it2 != it->end(); ++it2){
                cerr << "  Token: "  << *(*it2) << endl;
            }
        }
*/
    } catch(const string& msg){
        // If an exception occurs print the message and end the program
        cerr << msg << endl;
    }
    // Delete the Tokens that have been made
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
        vector<Token*>::iterator it2;
        for(it2 = it->begin(); it2 != it->end(); ++it2){
            delete *it2;
        }
    }
}
