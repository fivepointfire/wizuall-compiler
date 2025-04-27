CC = gcc
CFLAGS = -I. -Wall -Wextra
LEX = flex
YACC = bison

# Source files
SRCS = core/main.c ir/ast_builder.c ir/codegen.c
OBJS = $(SRCS:.c=.o)
LEX_SRC = lexer/wizuall_lexer.l
YACC_SRC = grammar/wizuall_parser.y

# Generated files
LEX_C = lexer/lex.yy.c
YACC_C = wizuall_parser.tab.c
YACC_H = wizuall_parser.tab.h

# Output binary
TARGET = wizuall_compiler

all: $(TARGET)

$(YACC_C) $(YACC_H): $(YACC_SRC)
	$(YACC) -d -o $(YACC_C) $(YACC_SRC)

$(LEX_C): $(LEX_SRC)
	$(LEX) -o $(LEX_C) $(LEX_SRC)

core/main.o: core/main.c ir/ast.h ir/codegen.h
ir/ast_builder.o: ir/ast_builder.c ir/ast.h
ir/codegen.o: ir/codegen.c ir/ast.h ir/codegen.h

$(TARGET): $(YACC_C) $(YACC_H) $(LEX_C) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) core/main.c $(LEX_C) $(YACC_C) ir/ast_builder.c ir/codegen.c -lfl

clean:
	rm -f $(TARGET) $(LEX_C) $(YACC_C) $(YACC_H) core/*.o ir/*.o output.py

.PHONY: all clean