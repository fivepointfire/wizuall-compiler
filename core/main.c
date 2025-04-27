#include <stdio.h>
#include <stdlib.h>
#include "../ir/ast.h"
#include "../ir/codegen.h"

// Declare parser function
extern int yyparse();
extern ASTNode* final_ast;

int main() {
    printf("Enter WizuAll code or feed file through < operator.\n");
    if (yyparse() == 0) {
        printf("\n✅ Parsing successful! Here's the AST:\n\n");
        printAST(final_ast, 0);
        FILE* out = fopen("output.py", "w");
        if (out) {
            generate_code(final_ast, out,0);
            fclose(out);
            printf("\n🚀 Python code generated in output.py\n");
        } else {
            printf("\n❌ Could not open output.py for writing.\n");
        }
    } else {
        printf("\n❌ Parsing failed.\n");
    }
    return 0;
}
