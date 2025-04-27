#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Flags for imports and helpers
static bool matplotlib_imported = false;
static bool seaborn_imported = false;
static bool numpy_imported = false;
static bool paretoset_emitted = false;
static bool pairwise_emitted = false;

// Forward declarations
void generate_code(ASTNode* node, FILE* out, int indent);
void generate_expr(ASTNode* node, FILE* out, int indent);
void emit_helpers(FILE* out);
void emit_imports(FILE* out);
void scan_for_imports_and_helpers(ASTNode* node);

void print_indent(FILE* out, int indent) {
    for (int i = 0; i < indent; i++)
        fprintf(out, "    ");
}

// Helper to check if a string matches
bool streq(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

// Scan AST for needed imports/helpers
void scan_for_imports_and_helpers(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM:
            for (ASTList* s = node->program.statements; s; s = s->next)
                scan_for_imports_and_helpers(s->node);
            break;
        case NODE_VIZ_CALL:
            matplotlib_imported = true;
            if (streq(node->viz_call.viz_func, "heatmap")) seaborn_imported = true;
            break;
        case NODE_FUNCTION_CALL: {
            const char* func = node->function_call.func_name;
            if (streq(func, "runningSum")) numpy_imported = true;
            if (streq(func, "paretoSet")) paretoset_emitted = true;
            if (streq(func, "pairwiseCompare")) pairwise_emitted = true;
            for (ASTList* arg = node->function_call.args; arg; arg = arg->next)
                scan_for_imports_and_helpers(arg->node);
            break;
        }
        case NODE_ASSIGNMENT:
            scan_for_imports_and_helpers(node->assignment.expr);
            break;
        case NODE_BINARY_OP:
            scan_for_imports_and_helpers(node->binary_op.left);
            scan_for_imports_and_helpers(node->binary_op.right);
            break;
        case NODE_VECTOR_LITERAL:
            for (ASTList* e = node->vector_literal.elements; e; e = e->next)
                scan_for_imports_and_helpers(e->node);
            break;
        case NODE_IF_ELSE:
            scan_for_imports_and_helpers(node->if_else.condition);
            for (ASTList* s = node->if_else.if_body; s; s = s->next)
                scan_for_imports_and_helpers(s->node);
            for (ASTList* s = node->if_else.else_body; s; s = s->next)
                scan_for_imports_and_helpers(s->node);
            break;
        case NODE_WHILE_LOOP:
            scan_for_imports_and_helpers(node->while_loop.condition);
            for (ASTList* s = node->while_loop.body; s; s = s->next)
                scan_for_imports_and_helpers(s->node);
            break;
        case NODE_FOR_LOOP:
            scan_for_imports_and_helpers(node->for_loop.init);
            scan_for_imports_and_helpers(node->for_loop.condition);
            scan_for_imports_and_helpers(node->for_loop.increment);
            for (ASTList* s = node->for_loop.body; s; s = s->next)
                scan_for_imports_and_helpers(s->node);
            break;
        default:
            break;
    }
}

void emit_imports(FILE* out) {
    if (matplotlib_imported) fprintf(out, "import matplotlib.pyplot as plt\n");
    if (numpy_imported) fprintf(out, "import numpy as np\n");
}

void emit_helpers(FILE* out) {
    if (pairwise_emitted) {
        fprintf(out, "def pairwise_compare(x):\n    return [x[i+1] - x[i] for i in range(len(x)-1)]\n\n");
    }
    if (paretoset_emitted) {
        fprintf(out, "def pareto_set(x):\n    # Dummy implementation: returns unique values\n    return list(set(x))\n\n");
    }
}

// Helper to emit plt.title, plt.xlabel, plt.ylabel after plot
void emit_labels(ASTList* args, FILE* out, int indent) {
    for (ASTList* a = args; a; a = a->next) {
        ASTNode* n = a->node;
        if (n->type == NODE_BINARY_OP && n->binary_op.op == OP_ASSIGN && n->binary_op.left->type == NODE_ID) {
            const char* key = n->binary_op.left->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(n->binary_op.right, out, indent);
                fprintf(out, ")\n");
            }
        }
    }
}

// Helper to emit Python for WizuAll built-in functions
void generate_builtin_func(const char* func, ASTList* args, FILE* out, int indent) {
    if (streq(func, "avg")) {
        fprintf(out, "(sum(");
        generate_expr(args->node, out, indent);
        fprintf(out, ") / len(");
        generate_expr(args->node, out, indent);
        fprintf(out, "))");
    } else if (streq(func, "sort")) {
        fprintf(out, "sorted(");
        generate_expr(args->node, out, indent);
        fprintf(out, ")");
    } else if (streq(func, "reverse")) {
        fprintf(out, "list(reversed(");
        generate_expr(args->node, out, indent);
        fprintf(out, "))");
    } else if (streq(func, "slice")) {
        ASTList* a1 = args;
        ASTList* a2 = a1 ? a1->next : NULL;
        ASTList* a3 = a2 ? a2->next : NULL;
        if (a1 && a2 && a3) {
            generate_expr(a1->node, out, indent);
            fprintf(out, "[");
            generate_expr(a2->node, out, indent);
            fprintf(out, ":");
            generate_expr(a3->node, out, indent);
            fprintf(out, "]");
        } else {
            fprintf(out, "# ERROR: slice expects 3 arguments");
        }
    } else if (streq(func, "transpose")) {
        fprintf(out, "list(map(list, zip(*");
        generate_expr(args->node, out, indent);
        fprintf(out, ")))" );
    } else if (streq(func, "runningSum")) {
        fprintf(out, "np.cumsum(");
        generate_expr(args->node, out, indent);
        fprintf(out, ")");
    } else if (streq(func, "pairwiseCompare")) {
        fprintf(out, "pairwise_compare(");
        generate_expr(args->node, out, indent);
        fprintf(out, ")");
    } else if (streq(func, "paretoSet")) {
        fprintf(out, "pareto_set(");
        generate_expr(args->node, out, indent);
        fprintf(out, ")");
    } else {
        fprintf(out, "%s(", func);
        for (ASTList* arg = args; arg; arg = arg->next) {
            generate_expr(arg->node, out, indent);
            if (arg->next) fprintf(out, ", ");
        }
        fprintf(out, ")  # WARNING: unknown function, passed through\n");
    }
}

// Helper to split positional and keyword args for visualizations
void split_viz_args(ASTList* args, ASTNode** pos_args, int* pos_count, ASTNode** kw_keys, ASTNode** kw_vals, int* kw_count) {
    *pos_count = 0;
    *kw_count = 0;
    for (ASTList* a = args; a; a = a->next) {
        ASTNode* n = a->node;
        if (n->type == NODE_BINARY_OP && n->binary_op.op == OP_ASSIGN && n->binary_op.left->type == NODE_ID) {
            kw_keys[*kw_count] = n->binary_op.left;
            kw_vals[*kw_count] = n->binary_op.right;
            (*kw_count)++;
        } else {
            pos_args[*pos_count] = n;
            (*pos_count)++;
        }
    }
}

void generate_viz_call(const char* func, ASTList* args, FILE* out, int indent) {
    // Support up to 10 positional/kw args for simplicity
    ASTNode* pos_args[10]; int pos_count = 0;
    ASTNode* kw_keys[10];  ASTNode* kw_vals[10]; int kw_count = 0;
    split_viz_args(args, pos_args, &pos_count, kw_keys, kw_vals, &kw_count);
    print_indent(out, indent);
    if (streq(func, "plot")) {
        fprintf(out, "plt.plot(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        fprintf(out, ")\n");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "histogram")) {
        fprintf(out, "plt.hist(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        fprintf(out, ")\n");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "scatter")) {
        fprintf(out, "plt.scatter(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        fprintf(out, ")\n");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "barchart")) {
        fprintf(out, "plt.bar(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        fprintf(out, ")\n");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "boxplot")) {
        fprintf(out, "plt.boxplot(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        fprintf(out, ")\n");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "heatmap")) {
        fprintf(out, "plt.imshow(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        bool has_cmap = false;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) continue;
            if (streq(key, "cmap")) has_cmap = true;
            fprintf(out, ", %s=", key);
            generate_expr(kw_vals[i], out, indent);
        }
        fprintf(out, ", aspect='auto'");
        if (!has_cmap) fprintf(out, ", cmap='viridis'");
        fprintf(out, ")\n");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        print_indent(out, indent);
        fprintf(out, "plt.colorbar()\n");
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "piechart")) {
        fprintf(out, "plt.pie(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        fprintf(out, ")\n");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel")) {
                print_indent(out, indent);
                fprintf(out, "plt.%s(", key);
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "timeline")) {
        fprintf(out, "# Timeline visualization not implemented\n");
    } else {
        fprintf(out, "# Unknown visualization: %s\n", func);
    }
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
        case NODE_FUNCTION_CALL: {
            const char* func = node->function_call.func_name;
            if (streq(func, "avg") || streq(func, "sort") || streq(func, "reverse") || streq(func, "slice") || streq(func, "transpose") || streq(func, "runningSum") || streq(func, "pairwiseCompare") || streq(func, "paretoSet")) {
                generate_builtin_func(func, node->function_call.args, out, indent);
            } else {
                fprintf(out, "%s(", func);
                for (ASTList* arg = node->function_call.args; arg; arg = arg->next) {
                    generate_expr(arg->node, out, indent);
                    if (arg->next) fprintf(out, ", ");
                }
                fprintf(out, ")");
            }
            break;
        }
        default:
            fprintf(out, "/* unsupported expr */");
    }
}

void generate_code(ASTNode* node, FILE* out, int indent) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM:
            // First pass: scan for needed imports/helpers
            scan_for_imports_and_helpers(node);
            emit_imports(out);
            emit_helpers(out);
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
        case NODE_VIZ_CALL:
            generate_viz_call(node->viz_call.viz_func, node->viz_call.args, out, indent);
            break;
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