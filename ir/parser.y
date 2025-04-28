%token IMPORT

%%

Statement
    : ImportStatement
    | Assignment SEMICOLON
    | FunctionCall SEMICOLON
    | VisualizationCall SEMICOLON
    | ControlStructure
    ;

ImportStatement
    : IMPORT STRING SEMICOLON { $$ = createImportNode($2); }
    ; 