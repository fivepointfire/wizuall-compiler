#include <stdio.h>
#include <stdlib.h>
#include "../ir/ast.h"

// Declare parser function
extern int yyparse();
extern ASTNode* final_ast;

int main() {
    printf("Enter WizuAll code or feed file through < operator.\n");
    if (yyparse() == 0) {
        printf("\n✅ Parsing successful! Here's the AST:\n\n");
        printAST(final_ast, 0);
    } else {
        printf("\n❌ Parsing failed.\n");
    }
    return 0;
}
