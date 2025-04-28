ASTNode* createImportNode(const char* filename) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_IMPORT;
    node->import.filename = strdup(filename);
    return node;
} 