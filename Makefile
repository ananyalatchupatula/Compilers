# ---------- Compiler ----------

CXX = g++
CXXFLAGS = -std=c++11 -Wall

# ---------- Tools ----------

LEX = flex
YACC = bison

# ---------- NEW COMPILER (class-based AST) ----------

NEW_TARGET = new-compiler
NEW_YACC_SRC = parser_new.y
NEW_YACC_CPP = parser_new.tab.cpp
NEW_YACC_HPP = parser_new.tab.hpp
NEW_LEX_OUT = lex_new.yy.c
NEW_OBJS = parser_new.tab.o lex_new.yy.o ast_new.o tac_str.o tac_generator.o rtl_new.o rtl_generator.o main_new.o

# ---------- OLD COMPILER (original) ----------

OLD_TARGET = old-compiler
OLD_YACC_SRC = parser.y
OLD_YACC_CPP = parser.tab.cpp
OLD_YACC_HPP = parser.tab.hpp
OLD_LEX_OUT = lex.yy.c
OLD_OBJS = parser.tab.o lex.yy.o ast.o tac.o main.o

# ---------- Default ----------

# Note: Default builds old-compiler (the stable, working version)
# Use 'make both' to build both versions
# Use 'make report' or 'make compare' for comparison
all: $(OLD_TARGET)
	@echo "✓ Old compiler built: $(OLD_TARGET)"
	@echo ""
	@echo "For comparison:"
	@echo "  make report   - Generate comprehensive comparison report"
	@echo "  make compare  - Detailed analysis"
	@echo "  make both     - Build both old and new versions"

# ============ NEW COMPILER (class-based AST) ============

# New compiler is the refactored version (parser_new.y + ast_new.cpp)
.PHONY: new-compiler
new-compiler: $(NEW_OBJS)
	$(CXX) $(CXXFLAGS) -o $(NEW_TARGET) $(NEW_OBJS)
	@echo "✓ New compiler compiled: $(NEW_TARGET)"

# New Bison
$(NEW_YACC_CPP) $(NEW_YACC_HPP): $(NEW_YACC_SRC)
	$(YACC) -d -o $(NEW_YACC_CPP) $(NEW_YACC_SRC)

# New Flex
$(NEW_LEX_OUT): scanner_new.l $(NEW_YACC_HPP)
	$(LEX) -o $(NEW_LEX_OUT) scanner_new.l

# New object files
parser_new.tab.o: parser_new.tab.cpp
	$(CXX) $(CXXFLAGS) -c parser_new.tab.cpp -o parser_new.tab.o

lex_new.yy.o: lex_new.yy.c
	$(CXX) $(CXXFLAGS) -c lex_new.yy.c -o lex_new.yy.o

ast_new.o: ast_new.cpp ast_new.h
	$(CXX) $(CXXFLAGS) -c ast_new.cpp

tac_new.o: tac_new.cpp tac_new.h
	$(CXX) $(CXXFLAGS) -c tac_new.cpp

tac_str.o: tac_str.cpp tac_str.h ast_new.h
	$(CXX) $(CXXFLAGS) -c tac_str.cpp

tac_generator.o: tac_generator.cpp tac_generator.h
	$(CXX) $(CXXFLAGS) -c tac_generator.cpp

rtl_new.o: rtl_new.cpp rtl_new.h
	$(CXX) $(CXXFLAGS) -c rtl_new.cpp

rtl_generator.o: rtl_generator.cpp rtl_generator.h
	$(CXX) $(CXXFLAGS) -c rtl_generator.cpp

main_new.o: main_new.cpp ast_new.h rtl_new.h rtl_generator.h
	$(CXX) $(CXXFLAGS) -c main_new.cpp -o main_new.o

# ============ OLD COMPILER (original implementation) ============

$(OLD_TARGET): $(OLD_OBJS)
	$(CXX) $(CXXFLAGS) -o $(OLD_TARGET) $(OLD_OBJS)
	@echo "✓ Old compiler compiled: $(OLD_TARGET)"

# Old Bison
$(OLD_YACC_CPP) $(OLD_YACC_HPP): $(OLD_YACC_SRC)
	$(YACC) -d -o parser.tab.cpp $(OLD_YACC_SRC)

# Old Flex
$(OLD_LEX_OUT): scanner.l $(OLD_YACC_HPP)
	$(LEX) scanner.l

# Old object files
parser.tab.o: parser.tab.cpp
	$(CXX) $(CXXFLAGS) -c parser.tab.cpp

lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c lex.yy.c

ast.o: ast.cpp ast.h
	$(CXX) $(CXXFLAGS) -c ast.cpp

tac.o: tac.cpp tac.h
	$(CXX) $(CXXFLAGS) -c tac.cpp

main.o: main.cpp ast.h
	$(CXX) $(CXXFLAGS) -c main.cpp

# ============ COMPARISON & TESTING ============

.PHONY: both
both: $(NEW_TARGET) $(OLD_TARGET)
	@echo ""
	@echo "✓ Both compilers built:"
	@echo "  New: $(NEW_TARGET)"
	@echo "  Old: $(OLD_TARGET)"

.PHONY: compare
compare:
	@if [ -f compare_compilers.sh ] && [ -f old-compiler ] && [ -f compiler ]; then \
		echo "Running comparison..."; \
		./compare_compilers.sh; \
	else \
		echo "Error: Missing required files"; \
		echo "  Need: compare_compilers.sh, old-compiler, compiler"; \
	fi

.PHONY: report
report:
	@if [ -f comparison_report.sh ] && [ -f old-compiler ] && [ -f compiler ]; then \
		echo "Generating comprehensive report..."; \
		./comparison_report.sh; \
	else \
		echo "Error: Missing required files"; \
		echo "  Need: comparison_report.sh, old-compiler, compiler"; \
	fi

.PHONY: help
help:
	@echo ""
	@echo "SCLP Compiler - Build and Comparison Targets:"
	@echo ""
	@echo "Build Targets:"
	@echo "  make old-compiler   - Build from original parser.y + ast.cpp (STABLE)"
	@echo "  make new-compiler   - Build from refactored parser_new.y (IN PROGRESS)"
	@echo "  make both           - Build both versions"
	@echo ""
	@echo "Comparison Targets:"
	@echo "  make compare        - Run detailed comparison on both compilers"
	@echo "  make report         - Generate comprehensive comparison report"
	@echo ""
	@echo "Cleanup:"
	@echo "  make clean          - Remove object files and generated sources"
	@echo "  make clean-all      - Remove everything including test artifacts"
	@echo "  make rebuild        - Clean and rebuild old-compiler (default)"
	@echo "  make rebuild-both   - Clean and rebuild both versions"
	@echo ""

# ============ CLEAN ============

.PHONY: clean
clean:
	rm -f *.o $(OLD_LEX_OUT) $(NEW_LEX_OUT) parser.tab.cpp parser.tab.hpp $(OLD_YACC_CPP) $(OLD_YACC_HPP) $(NEW_YACC_CPP) $(NEW_YACC_HPP) $(OLD_TARGET) $(NEW_TARGET)
	@echo "✓ Cleaned object files and generated files"

.PHONY: clean-all
clean-all: clean
	rm -f test_case_*.ast.* test_case_*.tac.* *.core
	@echo "✓ Cleaned test artifacts"

# ============ REBUILD ============

.PHONY: rebuild
rebuild: clean all

.PHONY: rebuild-all
rebuild-all: clean-all all
