#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static bool matplotlib_imported = false;

void generate_code(ASTNode* node, FILE* out, int indent);
void generate_expr(ASTNode* node, FILE* out, int indent);

void print_indent(FILE* out, int indent) {
    for (int i = 0; i < indent; i++)
        fprintf(out, "    ");
}

void generate_expr(ASTNode* node, FILE* out, int indent) {
    if (!node) return;
    switch (node->type) {
        case NODE_NUMBER:
            fprintf(out, "%g", node->num_value);
            break;
        case NODE_ID:
            fprintf(out, "%s", node->id_name);
            break;
        case NODE_STRING:
            fprintf(out, "%s", node->id_name); // Already quoted in lexer
            break;
        case NODE_VECTOR_LITERAL: {
            fprintf(out, "[");
            ASTList* e = node->vector_literal.elements;
            while (e) {
                generate_expr(e->node, out, indent);
                if (e->next) fprintf(out, ", ");
                e = e->next;
            }
            fprintf(out, "]");
            break;
        }
        case NODE_BINARY_OP:
            generate_expr(node->binary_op.left, out, indent);
            switch (node->binary_op.op) {
                case OP_PLUS: fprintf(out, " + "); break;
                case OP_MINUS: fprintf(out, " - "); break;
                case OP_TIMES: fprintf(out, " * "); break;
                case OP_DIVIDE: fprintf(out, " / "); break;
                case OP_LT: fprintf(out, " < "); break;
                case OP_GT: fprintf(out, " > "); break;
                case OP_ASSIGN: fprintf(out, " = "); break;
                default: fprintf(out, " ? "); break;
            }
            generate_expr(node->binary_op.right, out, indent);
            break;
        case NODE_FUNCTION_CALL:
            fprintf(out, "%s(", node->function_call.func_name);
            for (ASTList* arg = node->function_call.args; arg; arg = arg->next) {
                generate_expr(arg->node, out, indent);
                if (arg->next) fprintf(out, ", ");
            }
            fprintf(out, ")");
            break;
        default:
            fprintf(out, "/* unsupported expr */");
    }
}

void generate_viz_args(ASTList* args, FILE* out, int indent) {
    for (ASTList* a = args; a; a = a->next) {
        ASTNode* n = a->node;
        if (n->type == NODE_BINARY_OP && n->binary_op.op == OP_ASSIGN) {
            // Keyword argument
            if (n->binary_op.left->type == NODE_ID) {
                fprintf(out, "%s=", n->binary_op.left->id_name);
                generate_expr(n->binary_op.right, out, indent);
            }
        } else {
            generate_expr(n, out, indent);
        }
        if (a->next) fprintf(out, ", ");
    }
}

void generate_code(ASTNode* node, FILE* out, int indent) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM:
            // Check if any visualization is present to import matplotlib
            if (!matplotlib_imported) {
                ASTList* s = node->program.statements;
                while (s) {
                    if (s->node && (s->node->type == NODE_VIZ_CALL)) {
                        fprintf(out, "import matplotlib.pyplot as plt\n");
                        matplotlib_imported = true;
                        break;
                    }
                    s = s->next;
                }
            }
            for (ASTList* s = node->program.statements; s; s = s->next)
                generate_code(s->node, out, indent);
            break;
        case NODE_ASSIGNMENT:
            print_indent(out, indent);
            fprintf(out, "%s = ", node->assignment.var_name);
            generate_expr(node->assignment.expr, out, indent);
            fprintf(out, "\n");
            break;
        case NODE_FUNCTION_CALL:
            print_indent(out, indent);
            generate_expr(node, out, indent);
            fprintf(out, "\n");
            break;
        case NODE_VIZ_CALL: {
            print_indent(out, indent);
            // Map WizuAll viz calls to matplotlib
            const char* func = node->viz_call.viz_func;
            if (strcmp(func, "plot") == 0) {
                fprintf(out, "plt.plot(");
                generate_viz_args(node->viz_call.args, out, indent);
                fprintf(out, ")\nplt.show()\n");
            } else if (strcmp(func, "histogram") == 0) {
                fprintf(out, "plt.hist(");
                generate_viz_args(node->viz_call.args, out, indent);
                fprintf(out, ")\nplt.show()\n");
            } else if (strcmp(func, "scatter") == 0) {
                fprintf(out, "plt.scatter(");
                generate_viz_args(node->viz_call.args, out, indent);
                fprintf(out, ")\nplt.show()\n");
            } else if (strcmp(func, "barchart") == 0) {
                fprintf(out, "plt.bar(");
                generate_viz_args(node->viz_call.args, out, indent);
                fprintf(out, ")\nplt.show()\n");
            } else if (strcmp(func, "boxplot") == 0) {
                fprintf(out, "plt.boxplot(");
                generate_viz_args(node->viz_call.args, out, indent);
                fprintf(out, ")\nplt.show()\n");
            } else if (strcmp(func, "heatmap") == 0) {
                fprintf(out, "import seaborn as sns\nplt.figure()\nsns.heatmap(");
                generate_viz_args(node->viz_call.args, out, indent);
                fprintf(out, ")\nplt.show()\n");
            } else if (strcmp(func, "piechart") == 0) {
                fprintf(out, "plt.pie(");
                generate_viz_args(node->viz_call.args, out, indent);
                fprintf(out, ")\nplt.show()\n");
            } else if (strcmp(func, "timeline") == 0) {
                fprintf(out, "# Timeline visualization not implemented\n");
            } else {
                fprintf(out, "# Unknown visualization: %s\n", func);
            }
            break;
        }
        case NODE_IF_ELSE:
            print_indent(out, indent);
            fprintf(out, "if ");
            generate_expr(node->if_else.condition, out, indent);
            fprintf(out, ":\n");
            for (ASTList* s = node->if_else.if_body; s; s = s->next)
                generate_code(s->node, out, indent + 1);
            print_indent(out, indent);
            fprintf(out, "else:\n");
            for (ASTList* s = node->if_else.else_body; s; s = s->next)
                generate_code(s->node, out, indent + 1);
            break;
        case NODE_WHILE_LOOP:
            print_indent(out, indent);
            fprintf(out, "while ");
            generate_expr(node->while_loop.condition, out, indent);
            fprintf(out, ":\n");
            for (ASTList* s = node->while_loop.body; s; s = s->next)
                generate_code(s->node, out, indent + 1);
            break;
        case NODE_FOR_LOOP:
            print_indent(out, indent);
            // For now, only support C-style for loops: for(init; cond; incr) { body }
            // We'll translate to Python's while loop
            generate_code(node->for_loop.init, out, indent);
            print_indent(out, indent);
            fprintf(out, "while ");
            generate_expr(node->for_loop.condition, out, indent);
            fprintf(out, ":\n");
            for (ASTList* s = node->for_loop.body; s; s = s->next)
                generate_code(s->node, out, indent + 1);
            generate_code(node->for_loop.increment, out, indent + 1);
            break;
        default:
            print_indent(out, indent);
            fprintf(out, "# unsupported node\n");
    }
} 