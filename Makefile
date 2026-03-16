# ---------- Compiler ----------

CXX = g++
CXXFLAGS = -std=c++17 -Wall

# ---------- Tools ----------

LEX = flex
YACC = bison

# ---------- Files ----------

TARGET = compiler

LEX_SRC = scanner.l
YACC_SRC = parser.y

LEX_OUT = lex.yy.c
YACC_CPP = parser.tab.cpp
YACC_HPP = parser.tab.hpp

OBJS = parser.tab.o lex.yy.o ast.o tac.o main.o

# ---------- Default ----------

all: $(TARGET)

# ---------- Build executable ----------

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# ---------- Bison ----------

$(YACC_CPP) $(YACC_HPP): $(YACC_SRC)
	$(YACC) -d -o parser.tab.cpp $(YACC_SRC)

# ---------- Flex ----------

$(LEX_OUT): $(LEX_SRC) $(YACC_HPP)
	$(LEX) $(LEX_SRC)

# ---------- Object files ----------

parser.tab.o: parser.tab.cpp
	$(CXX) $(CXXFLAGS) -c parser.tab.cpp

lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c lex.yy.c

ast.o: ast.cpp ast.h
	$(CXX) $(CXXFLAGS) -c ast.cpp

tac.o: tac.cpp tac.h
	$(CXX) $(CXXFLAGS) -c tac.cpp

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

# ---------- Clean ----------

clean:
	rm -f *.o $(LEX_OUT) parser.tab.cpp parser.tab.hpp $(TARGET)

# ---------- Rebuild ----------

rebuild: clean all
