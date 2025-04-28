#ifndef AST_H
#define AST_H

// Types of AST nodes
typedef enum {
    NODE_PROGRAM,        // New type for the Program (list of statements)
    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_VECTOR_LITERAL,
    NODE_NUMBER,
    NODE_ID,
    NODE_STRING,
    NODE_FUNCTION_CALL,
    NODE_VIZ_CALL,
    NODE_IF_ELSE,
    NODE_WHILE_LOOP,
    NODE_FOR_LOOP,
    NODE_AUX_BLOCK,
    NODE_IMPORT
} NodeType;

// Operators for binary expressions
typedef enum {
    OP_PLUS,
    OP_MINUS,
    OP_TIMES,
    OP_DIVIDE,
    OP_ASSIGN,
    OP_LT,
    OP_GT
} BinaryOpType;

// Forward declarations
struct ASTNode;

typedef struct ASTList {
    struct ASTNode* node;
    struct ASTList* next;
} ASTList;

// AST Node structure
typedef struct ASTNode {
    NodeType type;
    union {
        double num_value;  // For numbers
        char* id_name;     // For identifiers

        struct {           // For assignments
            char* var_name;
            struct ASTNode* expr;
        } assignment;

        struct {           // For binary operations
            BinaryOpType op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;

        struct {           // For vector literals
            ASTList* elements;
        } vector_literal;

        struct {           // For function calls
            char* func_name;
            ASTList* args;
        } function_call;

        struct {           // For visualization calls
            char* viz_func;
            ASTList* args;
        } viz_call;

        struct {           // For if-else
            struct ASTNode* condition;
            ASTList* if_body;
            ASTList* else_body;
        } if_else;

        struct {           // For while loop
            struct ASTNode* condition;
            ASTList* body;
        } while_loop;

        struct {           // For for loop
            struct ASTNode* init;
            struct ASTNode* condition;
            struct ASTNode* increment;
            ASTList* body;
        } for_loop;

        struct {           // For aux blocks
            char* raw_code;
        } aux_block;

        struct {           // For the Program (list of statements)
            ASTList* statements;
        } program;

        struct {           // For import statements
            char* filename;
        } import;

    };
} ASTNode;

// Function declarations
ASTNode* createProgramNode(ASTList* stmts);
ASTNode* createAssignmentNode(char* name, ASTNode* expr);
ASTNode* createBinaryOpNode(BinaryOpType op, ASTNode* left, ASTNode* right);
ASTNode* createVectorNode(ASTList* elements);
ASTNode* createNumberNode(double value);
ASTNode* createIdNode(char* name);
ASTNode* createStringNode(char* value);
ASTNode* createFunctionCallNode(char* name, ASTList* args);
ASTNode* createVizCallNode(char* name, ASTList* args);
ASTNode* createIfElseNode(ASTNode* cond, ASTList* if_body, ASTList* else_body);
ASTNode* createWhileNode(ASTNode* cond, ASTList* body);
ASTNode* createForNode(ASTNode* init, ASTNode* cond, ASTNode* incr, ASTList* body);
ASTNode* createAuxBlockNode(char* code);
ASTNode* createImportNode(const char* filename);

ASTList* createASTList(ASTNode* node);
ASTList* appendASTList(ASTList* list, ASTNode* node);

void printAST(ASTNode* node, int level);

#endif
