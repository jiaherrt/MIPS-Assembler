#include <vector>
#include <string>
#include <iostream>
#include <bitset>
#include <stdexcept>
#include <map>
#include <cassert>
#include "assembler.h"
#include "lexer.h"
#include "kind.h"
#include "utils.h"
using std::vector;
using std::string;
using std::ostream;
using std::bitset;
using std::cout;
using std::endl;
using std::cerr;
using std::map;
using std::out_of_range;


void ASM::Assembler::Pass1(vector<Token*>& line) {
	vector<Token*> templine;
	if (line[0]->getKind() == LABEL) {
		vector<Token*>::iterator it = LabelPass1(line);
		if (it == line.end()) return;
		else {
			for (; it != line.end(); it++)
				templine.push_back(*it);
			line = templine;
		}
	}

	if (line[0]->getKind() == DOTWORD) {
		instCount++;
		return;
	}
	else if (line[0]->getKind() == ID) {
		string name = line[0]->getLexeme();
		if 		(name == "add") 	instCount++;
		else if (name == "sub") 	instCount++;
		else if (name == "mult") 	instCount++;
		else if (name == "multu") 	instCount++;
		else if (name == "div") 	instCount++;
		else if (name == "divu") 	instCount++;
		else if (name == "mfhi") 	instCount++;
		else if (name == "mflo")	instCount++;
		else if (name == "lis") 	instCount++;
		else if (name == "lw") 		instCount++;
		else if (name == "sw") 		instCount++;
		else if (name == "slt") 	instCount++;	
		else if (name == "sltu") 	instCount++;
		else if (name == "beq") 	instCount++;
		else if (name == "bne") 	instCount++;
		else if (name == "jr") 		instCount++;
		else if (name == "jalr") 	instCount++;
		else throw string("ERROR: Invalid MIPS Instruction");
	}
	else throw string("ERROR: Invalid MIPS Instruction");
}


void ASM::Assembler::Pass2(vector<Token*>& line) {
	vector<Token*> templine;
	if (line[0]->getKind() == LABEL) {
		vector<Token*>::iterator it = LabelPass2(line);
		if (it == line.end()) return;
		else {
			for (; it != line.end(); it++)
				templine.push_back(*it);
			line = templine;
		}
	}

	if (line[0]->getKind() == DOTWORD) {
		PCCount++;
		DotWord(line);
	}
	else if (line[0]->getKind() == ID) {
		string name = line[0]->getLexeme();
		if 		(name == "add")		{ PCCount++;	ADD(line);	}
		else if (name == "sub") 	{ PCCount++;	SUB(line);	}
		else if (name == "mult") 	{ PCCount++;	MULT(line);	}
		else if (name == "multu") 	{ PCCount++;	MULTU(line);}
		else if (name == "div") 	{ PCCount++;	DIV(line);	}
		else if (name == "divu") 	{ PCCount++;	DIVU(line);	}
		else if (name == "mfhi") 	{ PCCount++;	MFHI(line);	}
		else if (name == "mflo")	{ PCCount++;	MFLO(line);	}
		else if (name == "lis") 	{ PCCount++;	LIS(line);	}
		else if (name == "lw") 		{ PCCount++;	LW(line);	}
		else if (name == "sw") 		{ PCCount++;	SW(line);	}
		else if (name == "slt") 	{ PCCount++;	SLT(line);	}	
		else if (name == "sltu") 	{ PCCount++;	SLTU(line);	}
		else if (name == "beq") 	{ PCCount++;	BEQ(line);	}
		else if (name == "bne") 	{ PCCount++;	BNE(line);	}
		else if (name == "jr") 		{ PCCount++;	JR(line);	}
		else if (name == "jalr") 	{ PCCount++;	JALR(line);	}
		else throw string("ERROR: Invalid MIPS Instruction");
	}
}


vector<ASM::Token*>::iterator ASM::Assembler::LabelPass1(vector<ASM::Token*>& label) {
	vector<ASM::Token*>::iterator it;
	for (it = label.begin(); it != label.end(); it++) {
		ASM::Token* token = *it;
		if (token->getKind() == LABEL) {
			string labelName = token->getLexeme();
			labelName = labelName.erase(labelName.length() - 1);
			try {
				lookupLabel(labelName);
				throw string("ERROR: Duplicate label found");
			}
			catch (out_of_range& oor) {
				symbolTable[labelName] = instCount * 4;
			}
		}
		else return it;
	}
	return it;
}


vector<ASM::Token*>::iterator ASM::Assembler::LabelPass2(vector<ASM::Token*>& label) {
	vector<ASM::Token*>::iterator it;
	for (it = label.begin(); it != label.end(); it++) {
		ASM::Token* token = *it;
		if (token->getKind() == LABEL) continue;
		else return it;
	}
	return it;
}


void ASM::Assembler::DotWord(vector<ASM::Token*>& word) {
	if (word.size() != 2)
		throw string("ERROR: .word instruction takes 1 operand");
	ASM::Token* number = word[1];
	if (number->getKind() == ID) {
		try {
			string num = number->getLexeme();
			int value = lookupLabel(num);
			binaryLines.push_back(value);
		}
		catch (out_of_range& oor) {
			throw string("ERROR: Label not defined");
		}
	}
	else if (number->getKind() == INT || number->getKind() == HEXINT) {
		binaryLines.push_back(number->toInt());
	}
	else {
		throw string("ERROR: Invalid operand");
	}
}


void ASM::Assembler::ADD(vector<ASM::Token*>& word) {
	const unsigned int numElements = 6;

	if (word.size() != numElements)
		throw string("ERROR: add instruction takes 3 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");

		reg_decval.push_back(value);
		i++;

		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");
	}

	string d = Utils::decToBinary5(reg_decval[0]);
	string s = Utils::decToBinary5(reg_decval[1]);
	string t = Utils::decToBinary5(reg_decval[2]);
	string add_inst = "000000" + s + t + d + "00000100000";

	assert(add_inst.length() == 32);
	binaryLines.push_back(add_inst);

}


void ASM::Assembler::SUB(vector<ASM::Token*>& word) {
	const unsigned int numElements = 6;

	if (word.size() != numElements)
		throw string("ERROR: sub instruction takes 3 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");

		reg_decval.push_back(value);
		i++;

		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");
	}

	string d = Utils::decToBinary5(reg_decval[0]);
	string s = Utils::decToBinary5(reg_decval[1]);
	string t = Utils::decToBinary5(reg_decval[2]);
	string sub_inst = "000000" + s + t + d + "00000100010";

	assert(sub_inst.length() == 32);
	binaryLines.push_back(sub_inst);
}


void ASM::Assembler::MULT(vector<ASM::Token*>& word) {
	const unsigned int numElements = 4;

	if (word.size() != numElements)
		throw string("ERROR: mult instruction takes 2 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string ("ERROR: invalid register value");

		reg_decval.push_back(value);
		i++;

		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");

	}

	string s = Utils::decToBinary5(reg_decval[0]);
	string t = Utils::decToBinary5(reg_decval[1]);
	string mult_inst = "000000" + s + t + "0000000000011000";

	assert(mult_inst.length() == 32);
	binaryLines.push_back(mult_inst);

}


void ASM::Assembler::MULTU(vector<ASM::Token*>& word) {
	const unsigned int numElements = 4;
	
	if (word.size() != numElements)
		throw string("ERROR: multu instruction takes 2 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");

		reg_decval.push_back(value);
		i++;

		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");

	}

	string s = Utils::decToBinary5(reg_decval[0]);
	string t = Utils::decToBinary5(reg_decval[1]);
	string multu_inst = "000000" + s + t + "0000000000011001";

	assert(multu_inst.length() == 32);
	binaryLines.push_back(multu_inst);

}


void ASM::Assembler::DIV(vector<ASM::Token*>& word) {
	const unsigned int numElements = 4;

	if (word.size() != numElements)
		throw string("ERROR: div instruction takes 2 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");

		reg_decval.push_back(value);
		i++;

		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");

	}

	string s = Utils::decToBinary5(reg_decval[0]);
	string t = Utils::decToBinary5(reg_decval[1]);
	string div_inst = "000000" + s + t + "0000000000011010";

	assert(div_inst.length() == 32);
	binaryLines.push_back(div_inst);

}


void ASM::Assembler::DIVU(vector<ASM::Token*>& word) {
	const unsigned int numElements = 4;

	if (word.size() != numElements)
		throw string("ERROR: divu instruction takes 2 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");

		reg_decval.push_back(value);
		i++;

		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");
	}

	string s = Utils::decToBinary5(reg_decval[0]);
	string t = Utils::decToBinary5(reg_decval[1]);
	string divu_inst = "000000" + s + t + "0000000000011011";

	assert(divu_inst.length() == 32);
	binaryLines.push_back(divu_inst);

}


void ASM::Assembler::MFHI(vector<ASM::Token*>& word) {
	const unsigned int numElements = 2;

	if (word.size() != numElements)
		throw string("ERROR: mfhi instruction take 1 operand");

	int value = word[1]->toInt();
	if (value > 32 || value < 0)
		throw string("ERROR: invalid register value");

	string d = Utils::decToBinary5(value);
	string mfhi_inst = "0000000000000000" + d + "00000010000";

	assert(mfhi_inst.length() == 32);
	binaryLines.push_back(mfhi_inst);

}


void ASM::Assembler::MFLO(vector<ASM::Token*>& word) {
	const unsigned int numElements = 2;

	if (word.size() != numElements)
		throw string("ERROR: mflo instruction take 1 operand");

	int value = word[1]->toInt();
	if (value > 32 || value < 0)
		throw string("ERROR: invalid register value");

	string d = Utils::decToBinary5(value);
	string mflo_inst = "0000000000000000" + d + "00000010010";

	assert(mflo_inst.length() == 32);
	binaryLines.push_back(mflo_inst);

}


void ASM::Assembler::LIS(vector<ASM::Token*>& word) {
	const unsigned int numElements = 2;

	if (word.size() != numElements)
		throw string("ERROR: lis instruction takes 1 operand");

	if (word[1]->getKind() != REGISTER)
		throw string("ERROR: invalid operand");

	int value = word[1]->toInt();

	if (value > 32 || value < 0)
		throw string("ERROR: invalid register value");

	string d = Utils::decToBinary5(value);
	string lis_inst = "0000000000000000" + d + "00000010100";

	assert(lis_inst.length() == 32);
	PCCount++;
	binaryLines.push_back(lis_inst);

}


void ASM::Assembler::LW(vector<ASM::Token*>& word) {
	const unsigned int numElements = 7;

	if (word.size() != numElements)
		throw string("ERROR: lw instruction takes 2 operands");
	vector<int> reg_decval;

	if (word[1]->getKind() != REGISTER)
		throw string("ERROR: invalid operand");
	
	reg_decval.push_back(word[1]->toInt());
	if (reg_decval[0] > 32 || reg_decval[0] < 0)
		throw string("ERROR: invalid register value");

	if (word[2]->getKind() != COMMA)
		throw string("ERROR: expecting a comma");

	if (word[3]->getKind() != INT && word[3]->getKind() != HEXINT)
		throw string("ERROR: invalid offset value");
	
	int value = word[3]->toInt();
	
	if (word[3]->getKind() == INT && !(value >= -32768 && value <= 32767))
		throw string("ERROR: offset out of bounds");
	else if (word[3]->getKind() == HEXINT && value > 0xffff)
		throw string("ERROR: offset out of bounds");

	if (word[4]->getKind() != LPAREN)
		throw string("ERROR: expecting left parenthesis");

	if (word[5]->getKind() != REGISTER)
		throw string("ERROR: invalid operand");

	reg_decval.push_back(word[5]->toInt());
	if (reg_decval[1] > 32 || reg_decval[1] < 0)
		throw string("ERROR: invalid register value");

	if (word[6]->getKind() != RPAREN)
		throw string("ERROR: expecting right parenthesis");

	string t = Utils::decToBinary5(reg_decval[0]);
	string i = Utils::decToBinary16(value);
	string s = Utils::decToBinary5(reg_decval[1]);
	string lw_inst = "100011" + s + t + i;

	assert(lw_inst.length() == 32);
	binaryLines.push_back(lw_inst);

}


void ASM::Assembler::SW(vector<ASM::Token*>& word) {
	const unsigned int numElements = 7;

	if (word.size() != numElements)
		throw string("ERROR: sw instruction takes 2 operands");
	
	vector<int> reg_decval;

	if (word[1]->getKind() != REGISTER)
		throw string("ERROR: invalid operand");

	reg_decval.push_back(word[1]->toInt());
	if (reg_decval[0] > 32 || reg_decval[0] < 0)
		throw string("ERROR: invalid register value");

	if (word[2]->getKind() != COMMA)
		throw string("ERROR: expecting a comma");
	if (word[3]->getKind() !=  INT && word[3]->getKind() != HEXINT)
		throw string("ERROR: invalid offset value");

	int value = word[3]->toInt();

	if (word[3]->getKind() == INT && !(value >= -32768 && value <= 32767))
		throw string("ERROR: offset out of bounds");
	else if (word[3]->getKind() == HEXINT && value > 0xffff)
		throw string("ERROR: offset out of bounds");

	if (word[4]->getKind() != LPAREN)
		throw string("ERROR: expecting left parenthesis");

	if (word[5]->getKind() != REGISTER)
		throw string("ERROR: invalid register value");

	reg_decval.push_back(word[5]->toInt());
	if (reg_decval[1] > 32 || reg_decval[1] < 0)
		throw string("ERROR: invalid register value");

	if (word[6]->getKind() != RPAREN)
		throw string("ERROR: expecting right parenthsis");

	string t = Utils::decToBinary5(reg_decval[0]);
	string i = Utils::decToBinary16(value);
	string s = Utils::decToBinary5(reg_decval[1]);
	string sw_inst = "101011" + s + t + i;

	assert(sw_inst.length() == 32);
	binaryLines.push_back(sw_inst);

}


void ASM::Assembler::SLT(vector<ASM::Token*>& word) {
	const unsigned int numElements = 6;

	if (word.size() != numElements)
		throw string("ERROR: slt instruction takes 3 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");
		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");
		reg_decval.push_back(value);
		i++;
		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");
	}

	string d = Utils::decToBinary5(reg_decval[0]);
	string s = Utils::decToBinary5(reg_decval[1]);
	string t = Utils::decToBinary5(reg_decval[2]);
	string slt_inst = "000000" + s + t + d + "00000101010";

	assert(slt_inst.length() == 32);
	binaryLines.push_back(slt_inst);

}


void ASM::Assembler::SLTU(vector<ASM::Token*>& word) {
	const unsigned int numElements = 6;

	if (word.size() != numElements)
		throw string("ERROR: sltu instruction takes 3 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");
		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");
		reg_decval.push_back(value);
		i++;
		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");
	}

	string d = Utils::decToBinary5(reg_decval[0]);
	string s = Utils::decToBinary5(reg_decval[1]);
	string t = Utils::decToBinary5(reg_decval[2]);
	string sltu_inst = "000000" + s + t + d + "00000101011";

	assert(sltu_inst.length() == 32);
	binaryLines.push_back(sltu_inst);
	
}


void ASM::Assembler::BEQ(vector<ASM::Token*>& word) {
	const unsigned int numElements = 6;

	if (word.size() != numElements)
		throw string("ERROR: beq instruction takes 3 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements - 1; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (!(value >= 0 && value < 32))
			throw string("ERROR: invalid register value");
		
		reg_decval.push_back(value);
		i++;
		
		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");
	}

	string i;

	if (word[numElements - 1]->getKind() == INT) {
		int value = word[numElements - 1]->toInt();
		if (value >= -32768 && value <= 32767)
			i = Utils::decToBinary16(value);
		else
			throw string("ERROR: offset out of bounds");

	}
	else if (word[numElements - 1]->getKind() == HEXINT) {
		int value = word[numElements - 1]->toInt();
		if (value > 0xffff)
			throw string("ERROR: offset out of bounds");
		i = Utils::decToBinary16(value);

	}
	else if (word[numElements - 1]->getKind() == ID) {
		try {
			string valuestr = word[numElements - 1]->getLexeme();
			int value = lookupLabel(valuestr);
			value = (value - PCCount * 4) / 4;
			assert(value <= 32767 && value >= -32768);
			i = Utils::decToBinary16(value);

		}

		catch(out_of_range& oor) {
			throw string("ERROR: Label not defined");
		}
	}
	else
		throw string("ERROR: invalid offset");

	string s = Utils::decToBinary5(reg_decval[0]);
	string t = Utils::decToBinary5(reg_decval[1]);
	string beq_inst = "000100" + s + t + i;

	assert(beq_inst.length() == 32);
	binaryLines.push_back(beq_inst);

}

void ASM::Assembler::BNE(vector<ASM::Token*>& word) {
	const unsigned int numElements = 6;

	if (word.size() != numElements)
		throw string("ERROR: bne instruction takes 3 operands");
	vector<int> reg_decval;

	for (unsigned int i = 1; i < numElements - 1; i++) {
		if (word[i]->getKind() != REGISTER)
			throw string("ERROR: invalid operand");

		int value = word[i]->toInt();
		if (value > 32 || value < 0)
			throw string("ERROR: invalid register value");

		reg_decval.push_back(value);
		i++;

		if (i < numElements && word[i]->getKind() != COMMA)
			throw string("ERROR: expecting a comma, but found none");
	}

	string i;

	if (word[numElements - 1]->getKind() == INT) {
		int value = word[numElements - 1]->toInt();
		if (value >= -32768 && value <= 32767)
			i = Utils::decToBinary16(value);
		else
			throw string("ERROR: offset out of bounds");

	}
	else if (word[numElements - 1]->getKind() == HEXINT) {
		int value = word[numElements - 1]->toInt();
		if (value > 0xffff)
			throw string("ERROR: offset out of bounds");
		i = Utils::decToBinary16(value);
	
	}
	else if (word[numElements - 1]->getKind() == ID) {
		try {
			string valuestr = word[numElements - 1]->getLexeme();
			int value = lookupLabel(valuestr);
			value = (value - PCCount * 4) / 4;
			assert(value <= 32767 && value >= -32768);
			i = Utils::decToBinary16(value);
		}

		catch(out_of_range& oor) {
			throw string("ERROR: Label not defined");
		}
	}
	else
		throw string("ERROR: invalid offset");

	string s = Utils::decToBinary5(reg_decval[0]);
	string t = Utils::decToBinary5(reg_decval[1]);
	string bne_inst = "000101" + s + t + i;

	assert(bne_inst.length() == 32);
	binaryLines.push_back(bne_inst);
}


void ASM::Assembler::JR(vector<ASM::Token*>& word) {
	if (word.size() != 2) 
		throw string("ERROR: jr instruction takes 1 operand");

	ASM::Token* pc = word[1];
	if (pc->getKind() != REGISTER)
		throw string("ERROR: invalid operand");

	int value = pc->toInt();
	if (value > 32 || value < 0)
		throw string("ERROR: invalid register value");

	string regval = Utils::decToBinary5(value);
	string jr_inst = "000000" + regval + "000000000000000001000";

	assert(jr_inst.length() == 32);
	PCCount = value / 4;
	binaryLines.push_back(jr_inst);

}


void ASM::Assembler::JALR(vector<ASM::Token*>& word) {
	if (word.size() != 2)
		throw string("ERROR: jalr instruction takes 1 operand");
	
	ASM::Token* pc = word[1];
	if (pc->getKind() != REGISTER)
		throw string("ERROR: invalid operand");

	int value = pc->toInt();
	if (value > 32 || value < 0)
		throw string("ERROR: invalid register value");

	string regval = Utils::decToBinary5(value);
	string jalr_inst = "000000" + regval + "000000000000000001001";
	assert(jalr_inst.length() == 32);
	PCCount = value / 4;
	binaryLines.push_back(jalr_inst);
}


int ASM::Assembler::lookupLabel(string& label) {
	map<string, int>::iterator it = symbolTable.find(label);
	if (it != symbolTable.end()) {
		return it->second;
	}
	throw out_of_range ("symbolTable");
}


void ASM::Assembler::printBinaryLines() {
	vector<Utils>::iterator it;
	for (it = binaryLines.begin(); it != binaryLines.end(); it++) {
		it->toStdout();
	}
}


void ASM::Assembler::printSymTable() {
	map<string, int>::iterator it;
	for (it = symbolTable.begin(); it != symbolTable.end(); it++) {
		cerr << it->first << " " << it->second << endl;
	}
}
