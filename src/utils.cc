#include <cstdio>
#include <bitset>
#include <vector>
#include <string>
#include "utils.h"
using std::bitset;
using std::vector;
using std::string;

ASM::Utils::Utils(int value): value(value) {
    binary = bitset<32>(value).to_string();
    for (int i = 0; i < 4; i++) {
        string temp;
        for (int j = 0; j < 8; j++)
            temp.push_back(binary[i * 8 + j]);
        bitSequence.push_back(binaryToChar(temp));
    }   
}

ASM::Utils::Utils(string binary) {
    for (int i = 0; i < 4; i++) {
        string temp;
        for (int j = 0; j < 8; j++)
            temp.push_back(binary[i * 8 + j]);
        bitSequence.push_back(binaryToChar(temp));
    }
}

string ASM::Utils::decToBinary32(const int value) {
    return bitset<32>(value).to_string();
}

string ASM::Utils::decToBinary16(const int value) {
    return bitset<16>(value).to_string();
}

string ASM::Utils::decToBinary5(const int value) {
    return bitset<5>(value).to_string();
}

int ASM::Utils::twoCompHextoDec16(const int value) {
    string binary = bitset<16>(value).to_string();
    if (binary[0] == '1')
        return -value;
    else
        return value;
}

string ASM::Utils::decToBinary32() {
    return bitset<32>(value).to_string();
}

string ASM::Utils::decToBinary16() {
    return bitset<16>(value).to_string();
}

string ASM::Utils::decToBinary5() {
    return bitset<32>(value).to_string();
}

void ASM::Utils::toStdout() {
    vector<int>::iterator it;
    for (it = bitSequence.begin(); it != bitSequence.end(); it++)
        putchar(*it);
}

int ASM::Utils::binaryToChar(string binary) {
    int temp = 0;
    int base = 1;
    string::reverse_iterator rit;
    for (rit = binary.rbegin(); rit != binary.rend(); rit++) {
        temp += base * ((*rit) - '0');
        base *= 2;
    }
    return temp;
}

int ASM::Utils::getInt() const {
    return value;
}
