rm -f lex.yy.c parser.tab.* A1-sclp
bison -d parser.y
flex scanner.l
gcc lex.yy.c parser.tab.c main.c -o A1-sclp
