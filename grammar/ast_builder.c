#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// --- Node Creation Functions ---

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

// --- AST List Functions ---

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

// --- Debug Print Functions (Optional) ---

void printAST(ASTNode* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("NodeType: %d\n", node->type);
    // Expand later if needed
}
