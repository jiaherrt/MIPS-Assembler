#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace ASM {
    // Utils class encapsulates the implementation of the internal
    // writing and conversion of binary, decimal, hexidecimal and
    // the writing of bits to stdout.
    class Utils {
    private:
        int value; // decimal value 
        std::string binary; // binary value in string
        std::vector<int> bitSequence; // actual 8 bit chunks of bits

        int binaryToChar(std::string binary);

    public:
        Utils(int value);
        Utils(std::string binary);

        int getInt() const;
        static std::string decToBinary32(const int value);
        static std::string decToBinary16(const int value);
        static std::string decToBinary5(const int value);
        static int twoCompHextoDec16(const int value);
        std::string decToBinary32();
        std::string decToBinary16();
        std::string decToBinary5();
        void toStdout();

    };
}


#endif
