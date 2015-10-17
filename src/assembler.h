#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>


namespace ASM {

	class Utils;
	class Token;
	class Assembler {
		std::map<std::string, int> symbolTable;
		std::vector<Utils> binaryLines;
		int instCount;
		int PCCount;

		// Private methods for Symbol Table
		int lookupLabel(std::string& label); // Raises an out_of_range exception if label is not found

		// Private methods for MIPS Instructions
		void DotWord(std::vector<Token*>& word);
		void ADD(std::vector<Token*>& word);
		void SUB(std::vector<Token*>& word);
		void MULT(std::vector<Token*>& word);
		void MULTU(std::vector<Token*>& word);
		void DIV(std::vector<Token*>& word);
		void DIVU(std::vector<Token*>& word);
		void MFHI(std::vector<Token*>& word);
		void MFLO(std::vector<Token*>& word);
		void LIS(std::vector<Token*>& word);
		void LW(std::vector<Token*>& word);
		void SW(std::vector<Token*>& word);
		void SLT(std::vector<Token*>& word);
		void SLTU(std::vector<Token*>& word);
		void BEQ(std::vector<Token*>& word);
		void BNE(std::vector<Token*>& word);
		void JR(std::vector<Token*>& word);
		void JALR(std::vector<Token*>& word);

		std::vector<ASM::Token*>::iterator LabelPass1(std::vector<Token*>& label);
		std::vector<ASM::Token*>::iterator LabelPass2(std::vector<Token*>& label);

	public:
		Assembler(): instCount(0), PCCount(0) {}
		void Pass1(std::vector<Token*>& line);
		void Pass2(std::vector<Token*>& line);
		void printBinaryLines();
		void printSymTable();
	
	};
}

#endif
