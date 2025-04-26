#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Node creation functions

ASTNode* createProgramNode(ASTList* stmts) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->program.statements = stmts;
    return node;
}

ASTNode* createAssignmentNode(char* name, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ASSIGNMENT;
    node->assignment.var_name = strdup(name);
    node->assignment.expr = expr;
    return node;
}

ASTNode* createBinaryOpNode(BinaryOpType op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->binary_op.op = op;
    node->binary_op.left = left;
    node->binary_op.right = right;
    return node;
}

ASTNode* createVectorNode(ASTList* elements) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VECTOR_LITERAL;
    node->vector_literal.elements = elements;
    return node;
}

ASTNode* createNumberNode(double value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_NUMBER;
    node->num_value = value;
    return node;
}

ASTNode* createIdNode(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ID;
    node->id_name = strdup(name);
    return node;
}

ASTNode* createFunctionCallNode(char* name, ASTList* args) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION_CALL;
    node->function_call.func_name = strdup(name);
    node->function_call.args = args;
    return node;
}

ASTNode* createVizCallNode(char* name, ASTList* args) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VIZ_CALL;
    node->viz_call.viz_func = strdup(name);
    node->viz_call.args = args;
    return node;
}

ASTNode* createIfElseNode(ASTNode* cond, ASTList* if_body, ASTList* else_body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_IF_ELSE;
    node->if_else.condition = cond;
    node->if_else.if_body = if_body;
    node->if_else.else_body = else_body;
    return node;
}

ASTNode* createWhileNode(ASTNode* cond, ASTList* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_WHILE_LOOP;
    node->while_loop.condition = cond;
    node->while_loop.body = body;
    return node;
}

ASTNode* createForNode(ASTNode* init, ASTNode* cond, ASTNode* incr, ASTList* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FOR_LOOP;
    node->for_loop.init = init;
    node->for_loop.condition = cond;
    node->for_loop.increment = incr;
    node->for_loop.body = body;
    return node;
}

ASTNode* createAuxBlockNode(char* code) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_AUX_BLOCK;
    node->aux_block.raw_code = strdup(code);
    return node;
}

// List creation functions

ASTList* createASTList(ASTNode* node) {
    ASTList* list = malloc(sizeof(ASTList));
    list->node = node;
    list->next = NULL;
    return list;
}

ASTList* appendASTList(ASTList* list, ASTNode* node) {
    if (!list) return createASTList(node);
    ASTList* temp = list;
    while (temp->next) temp = temp->next;
    temp->next = createASTList(node);
    return list;
}

// Debug print function

void printAST(ASTNode* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");

    switch (node->type) {
        case NODE_PROGRAM:
            printf("Program\n");
            for (ASTList* s = node->program.statements; s; s = s->next)
                printAST(s->node, level+1);
            break;
        case NODE_ASSIGNMENT:
            printf("Assignment to %s\n", node->assignment.var_name);
            printAST(node->assignment.expr, level+1);
            break;
        case NODE_BINARY_OP:
            printf("BinaryOp ");
            switch (node->binary_op.op) {
                case OP_PLUS: printf("(+)\n"); break;
                case OP_MINUS: printf("(-)\n"); break;
                case OP_TIMES: printf("(*)\n"); break;
                case OP_DIVIDE: printf("(/)\n"); break;
                case OP_ASSIGN: printf("(=)\n"); break;
                case OP_LT: printf("(<)\n"); break;
                case OP_GT: printf("(>)\n"); break;
                default: printf("(Unknown BinaryOp)\n"); break;
            }
            printAST(node->binary_op.left, level+1);
            printAST(node->binary_op.right, level+1);
            break;
        case NODE_NUMBER:
            printf("Number: %lf\n", node->num_value);
            break;
        case NODE_ID:
            printf("Identifier: %s\n", node->id_name);
            break;
        case NODE_VECTOR_LITERAL:
            printf("VectorLiteral\n");
            for (ASTList* e = node->vector_literal.elements; e; e = e->next)
                printAST(e->node, level+1);
            break;
        case NODE_FUNCTION_CALL:
            printf("FunctionCall: %s\n", node->function_call.func_name);
            for (ASTList* a = node->function_call.args; a; a = a->next)
                printAST(a->node, level+1);
            break;
        case NODE_VIZ_CALL:
            printf("VisualizationCall: %s\n", node->viz_call.viz_func);
            for (ASTList* v = node->viz_call.args; v; v = v->next)
                printAST(v->node, level+1);
            break;
        case NODE_IF_ELSE:
            printf("IfElse\n");
            printAST(node->if_else.condition, level+1);
            printf("IfBody:\n");
            for (ASTList* ifb = node->if_else.if_body; ifb; ifb = ifb->next)
                printAST(ifb->node, level+2);
            printf("ElseBody:\n");
            for (ASTList* elseb = node->if_else.else_body; elseb; elseb = elseb->next)
                printAST(elseb->node, level+2);
            break;
        case NODE_WHILE_LOOP:
            printf("WhileLoop\n");
            printAST(node->while_loop.condition, level+1);
            for (ASTList* wb = node->while_loop.body; wb; wb = wb->next)
                printAST(wb->node, level+2);
            break;
        case NODE_FOR_LOOP:
            printf("ForLoop\n");
            printAST(node->for_loop.init, level+1);
            printAST(node->for_loop.condition, level+1);
            printAST(node->for_loop.increment, level+1);
            for (ASTList* fb = node->for_loop.body; fb; fb = fb->next)
                printAST(fb->node, level+2);
            break;
        case NODE_AUX_BLOCK:
            printf("AuxiliaryCodeBlock\n");
            break;
        default:
            printf("Unknown Node Type\n");
    }
}
