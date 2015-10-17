CXX = g++
CXXFLAGS = -Wall -MMD -g
OBJECTS = asm.o assembler.o utils.o kind.o lexer.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = asm

${EXEC}: ${OBJECTS}
        ${CXX} ${OBJECTS} -o ${EXEC}
-include ${DEPENDS}

.PHONY: clean
clean:
    rm ${DEPENDS} ${OBJECTS} ${EXEC}
