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
    
    // Helper function to generate positional arguments
    void generate_pos_args(int count) {
        for (int i = 0; i < count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
    }
    
    // Helper function to generate keyword arguments
    void generate_kw_args(const char* key, ASTNode* val, bool is_first) {
        if (!is_first) fprintf(out, ", ");
        fprintf(out, "%s=", key);
        generate_expr(val, out, indent);
    }
    
    if (streq(func, "plot")) {
        // Plot visualization with extended parameters
        fprintf(out, "plt.plot(");
        
        // Generate positional arguments first
        generate_pos_args(pos_count);
        
        // Add keyword arguments
        bool has_color = false, has_label = false, has_linestyle = false, 
             has_marker = false, has_markersize = false, has_linewidth = false;
        
        bool first_kw = true;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || 
                streq(key, "grid") || streq(key, "legend")) continue;
            
            if (streq(key, "color")) has_color = true;
            if (streq(key, "label")) has_label = true;
            if (streq(key, "linestyle")) has_linestyle = true;
            if (streq(key, "marker")) has_marker = true;
            if (streq(key, "markersize")) has_markersize = true;
            if (streq(key, "linewidth")) has_linewidth = true;
            
            // Add comma before first keyword arg only if we had positional args
            if (first_kw && pos_count > 0) fprintf(out, ", ");
            generate_kw_args(key, kw_vals[i], first_kw);
            first_kw = false;
        }
        
        // Add default values if not provided
        if (!has_color) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "color='blue'");
            first_kw = false;
        }
        if (!has_linestyle) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "linestyle='-'");
            first_kw = false;
        }
        if (!has_marker) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "marker=''");
            first_kw = false;
        }
        if (!has_markersize) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "markersize=5");
            first_kw = false;
        }
        if (!has_linewidth) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "linewidth=2");
        }
        
        fprintf(out, ")\n");
        
        // Add title, labels, grid, and legend
        bool has_title = false, has_xlabel = false, has_ylabel = false, has_grid = false, has_legend = false;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "xlabel")) {
                has_xlabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.xlabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "ylabel")) {
                has_ylabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.ylabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "grid")) {
                has_grid = true;
                print_indent(out, indent);
                fprintf(out, "plt.grid(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "legend")) {
                has_legend = true;
                print_indent(out, indent);
                fprintf(out, "plt.legend(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        
        // Add default values if not provided
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Plot')\n");
        }
        if (!has_xlabel) {
            print_indent(out, indent);
            fprintf(out, "plt.xlabel('X-axis')\n");
        }
        if (!has_ylabel) {
            print_indent(out, indent);
            fprintf(out, "plt.ylabel('Y-axis')\n");
        }
        if (!has_grid) {
            print_indent(out, indent);
            fprintf(out, "plt.grid(True)\n");
        }
        if (!has_legend && has_label) {
            print_indent(out, indent);
            fprintf(out, "plt.legend()\n");
        }
        
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "histogram")) {
        // Histogram visualization with extended parameters
        fprintf(out, "plt.hist(");
        
        // Generate positional arguments first
        generate_pos_args(pos_count);
        
        // Add keyword arguments
        bool has_bins = false, has_color = false, has_edgecolor = false, has_density = false;
        
        bool first_kw = true;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || 
                streq(key, "grid")) continue;
            
            if (streq(key, "bins")) has_bins = true;
            if (streq(key, "color")) has_color = true;
            if (streq(key, "edgecolor")) has_edgecolor = true;
            if (streq(key, "density")) has_density = true;
            
            // Add comma before first keyword arg only if we had positional args
            if (first_kw && pos_count > 0) fprintf(out, ", ");
            generate_kw_args(key, kw_vals[i], first_kw);
            first_kw = false;
        }
        
        // Add default values if not provided
        if (!has_bins) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "bins=10");
            first_kw = false;
        }
        if (!has_color) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "color='skyblue'");
            first_kw = false;
        }
        if (!has_edgecolor) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "edgecolor='black'");
            first_kw = false;
        }
        if (!has_density) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "density=False");
        }
        
        fprintf(out, ")\n");
        
        // Add title, labels, and grid
        bool has_title = false, has_xlabel = false, has_ylabel = false, has_grid = false;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "xlabel")) {
                has_xlabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.xlabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "ylabel")) {
                has_ylabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.ylabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "grid")) {
                has_grid = true;
                print_indent(out, indent);
                fprintf(out, "plt.grid(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        
        // Add default values if not provided
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Histogram')\n");
        }
        if (!has_xlabel) {
            print_indent(out, indent);
            fprintf(out, "plt.xlabel('Value')\n");
        }
        if (!has_ylabel) {
            print_indent(out, indent);
            fprintf(out, "plt.ylabel('Frequency')\n");
        }
        if (!has_grid) {
            print_indent(out, indent);
            fprintf(out, "plt.grid(True)\n");
        }
        
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "heatmap")) {
        // Heatmap visualization with extended parameters
        fprintf(out, "plt.imshow(");
        
        // Generate positional arguments first
        generate_pos_args(pos_count);
        
        // Add keyword arguments
        bool has_cmap = false, has_interpolation = false, has_aspect = false;
        bool first_kw = true;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || 
                streq(key, "colorbar")) continue;
            
            if (streq(key, "cmap")) has_cmap = true;
            if (streq(key, "interpolation")) has_interpolation = true;
            if (streq(key, "aspect")) has_aspect = true;
            
            // Add comma before first keyword arg only if we had positional args
            if (first_kw && pos_count > 0) fprintf(out, ", ");
            generate_kw_args(key, kw_vals[i], first_kw);
            first_kw = false;
        }
        
        // Add default values if not provided
        if (!has_cmap) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "cmap='viridis'");
            first_kw = false;
        }
        if (!has_interpolation) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "interpolation='nearest'");
            first_kw = false;
        }
        if (!has_aspect) {
            if (!first_kw || pos_count > 0) fprintf(out, ", ");
            fprintf(out, "aspect='auto'");
        }
        
        fprintf(out, ")\n");
        
        // Add title, labels, and colorbar
        bool has_title = false, has_xlabel = false, has_ylabel = false, has_colorbar = false;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "xlabel")) {
                has_xlabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.xlabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "ylabel")) {
                has_ylabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.ylabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "colorbar")) {
                has_colorbar = true;
                print_indent(out, indent);
                fprintf(out, "plt.colorbar(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        
        // Add default values if not provided
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Heatmap')\n");
        }
        if (!has_xlabel) {
            print_indent(out, indent);
            fprintf(out, "plt.xlabel('X-axis')\n");
        }
        if (!has_ylabel) {
            print_indent(out, indent);
            fprintf(out, "plt.ylabel('Y-axis')\n");
        }
        if (!has_colorbar) {
            print_indent(out, indent);
            fprintf(out, "plt.colorbar()\n");
        }
        
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "barchart")) {
        // Bar chart visualization with extended parameters
        fprintf(out, "plt.bar(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        // Only add a comma if there is at least one keyword argument to emit
        bool has_kwarg = false;
        bool has_color = false;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (!(streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || streq(key, "grid"))) {
                has_kwarg = true;
            }
            if (streq(key, "color")) has_color = true;
        }
        if (has_kwarg) {
            fprintf(out, ", ");
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || streq(key, "grid")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        // Add default values if not provided
        if (!has_color) fprintf(out, ", color='orange'");
        fprintf(out, ")\n");
        
        // Add title, labels, and grid
        bool has_title = false, has_xlabel = false, has_ylabel = false, has_grid = false;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "xlabel")) {
                has_xlabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.xlabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "ylabel")) {
                has_ylabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.ylabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "grid")) {
                has_grid = true;
                print_indent(out, indent);
                fprintf(out, "plt.grid(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        
        // Add default values if not provided
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Bar Chart')\n");
        }
        if (!has_xlabel) {
            print_indent(out, indent);
            fprintf(out, "plt.xlabel('Categories')\n");
        }
        if (!has_ylabel) {
            print_indent(out, indent);
            fprintf(out, "plt.ylabel('Values')\n");
        }
        if (!has_grid) {
            print_indent(out, indent);
            fprintf(out, "plt.grid(axis='y')\n");
        }
        
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "piechart")) {
        // Pie chart visualization with only values and labels
        fprintf(out, "plt.pie(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        // Only add a comma if there is at least one keyword argument to emit
        bool has_labels = false;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "labels")) {
                has_labels = true;
                break;
            }
        }
        if (has_labels && pos_count > 0) fprintf(out, ", ");
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "labels")) {
                fprintf(out, "labels=");
                generate_expr(kw_vals[i], out, indent);
            }
        }
        fprintf(out, ")\n");
        // Add title if provided
        bool has_title = false;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Pie Chart')\n");
        }
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "scatter")) {
        // Scatter plot visualization with extended parameters
        fprintf(out, "plt.scatter(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        // Only add a comma if there is at least one keyword argument to emit
        bool has_scatter_kwarg = false;
        bool has_color = false, has_marker = false, has_size = false, has_alpha = false;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (!(streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || streq(key, "grid"))) {
                has_scatter_kwarg = true;
            }
            if (streq(key, "color")) has_color = true;
            if (streq(key, "marker")) has_marker = true;
            if (streq(key, "s")) has_size = true;
            if (streq(key, "alpha")) has_alpha = true;
        }
        if (has_scatter_kwarg) {
            fprintf(out, ", ");
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || streq(key, "grid")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        // Add default values if not provided
        if (!has_color) fprintf(out, ", color='blue'");
        if (!has_marker) fprintf(out, ", marker='o'");
        if (!has_size) fprintf(out, ", s=100");
        if (!has_alpha) fprintf(out, ", alpha=0.6");
        
        fprintf(out, ")\n");
        
        // Add title, labels, and grid
        bool has_title = false, has_xlabel = false, has_ylabel = false, has_grid = false;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "xlabel")) {
                has_xlabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.xlabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "ylabel")) {
                has_ylabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.ylabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "grid")) {
                has_grid = true;
                print_indent(out, indent);
                fprintf(out, "plt.grid(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        
        // Add default values if not provided
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Scatter Plot')\n");
        }
        if (!has_xlabel) {
            print_indent(out, indent);
            fprintf(out, "plt.xlabel('X-axis')\n");
        }
        if (!has_ylabel) {
            print_indent(out, indent);
            fprintf(out, "plt.ylabel('Y-axis')\n");
        }
        if (!has_grid) {
            print_indent(out, indent);
            fprintf(out, "plt.grid(True)\n");
        }
        
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "boxplot")) {
        // Box plot visualization with extended parameters
        fprintf(out, "plt.boxplot(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        // Only add a comma if there is at least one keyword argument to emit
        bool has_boxplot_kwarg = false;
        bool has_notch = false, has_vert = false, has_patch_artist = false, has_tick_labels = false;
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (!(streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || streq(key, "grid"))) {
                has_boxplot_kwarg = true;
            }
            if (streq(key, "notch")) has_notch = true;
            if (streq(key, "vert")) has_vert = true;
            if (streq(key, "patch_artist")) has_patch_artist = true;
            if (streq(key, "tick_labels")) has_tick_labels = true;
        }
        if (has_boxplot_kwarg) {
            fprintf(out, ", ");
        }
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || streq(key, "grid")) continue;
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        // Add default values if not provided
        if (!has_notch) fprintf(out, ", notch=False");
        if (!has_vert) fprintf(out, ", vert=True");
        if (!has_patch_artist) fprintf(out, ", patch_artist=True");
        if (!has_tick_labels) {
            int n_labels = 1;
            if (pos_count > 0 && pos_args[0]->type == NODE_VECTOR_LITERAL) {
                // Count elements in the vector literal
                ASTList* e = pos_args[0]->vector_literal.elements;
                n_labels = 0;
                while (e) {
                    n_labels++;
                    e = e->next;
                }
            } else {
                n_labels = pos_count;
            }
            fprintf(out, ", tick_labels=[");
            for (int i = 0; i < n_labels; ++i) {
                if (i > 0) fprintf(out, ", ");
                fprintf(out, "'Data %d'", i+1);
            }
            fprintf(out, "]");
        }
        fprintf(out, ")\n");
        
        // Add title, labels, and grid
        bool has_title = false, has_xlabel = false, has_ylabel = false, has_grid = false;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "xlabel")) {
                has_xlabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.xlabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "ylabel")) {
                has_ylabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.ylabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "grid")) {
                has_grid = true;
                print_indent(out, indent);
                fprintf(out, "plt.grid(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        
        // Add default values if not provided
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Box Plot')\n");
        }
        if (!has_xlabel) {
            print_indent(out, indent);
            fprintf(out, "plt.xlabel('Groups')\n");
        }
        if (!has_ylabel) {
            print_indent(out, indent);
            fprintf(out, "plt.ylabel('Values')\n");
        }
        if (!has_grid) {
            print_indent(out, indent);
            fprintf(out, "plt.grid(True)\n");
        }
        
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else if (streq(func, "timeline")) {
        // Timeline visualization with extended parameters
        fprintf(out, "plt.plot(");
        for (int i = 0; i < pos_count; ++i) {
            if (i > 0) fprintf(out, ", ");
            generate_expr(pos_args[i], out, indent);
        }
        
        // Add keyword arguments
        bool has_color = false;
        
        // Add a comma after positional arguments if we have keyword arguments
        if (kw_count > 0) {
            fprintf(out, ", ");
        }
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title") || streq(key, "xlabel") || streq(key, "ylabel") || 
                streq(key, "grid") || streq(key, "autofmt_xdate")) continue;
            
            if (streq(key, "color")) has_color = true;
            
            fprintf(out, "%s=", key);
            generate_expr(kw_vals[i], out, indent);
            if (i < kw_count - 1) fprintf(out, ", ");
        }
        
        // Add default values if not provided
        if (!has_color) fprintf(out, ", color='purple'");
        
        fprintf(out, ")\n");
        
        // Add title, labels, grid, and autofmt_xdate
        bool has_title = false, has_xlabel = false, has_ylabel = false, has_grid = false, has_autofmt_xdate = false;
        
        for (int i = 0; i < kw_count; ++i) {
            const char* key = kw_keys[i]->id_name;
            if (streq(key, "title")) {
                has_title = true;
                print_indent(out, indent);
                fprintf(out, "plt.title(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "xlabel")) {
                has_xlabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.xlabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "ylabel")) {
                has_ylabel = true;
                print_indent(out, indent);
                fprintf(out, "plt.ylabel(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "grid")) {
                has_grid = true;
                print_indent(out, indent);
                fprintf(out, "plt.grid(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            } else if (streq(key, "autofmt_xdate")) {
                has_autofmt_xdate = true;
                print_indent(out, indent);
                fprintf(out, "plt.gcf().autofmt_xdate(");
                generate_expr(kw_vals[i], out, indent);
                fprintf(out, ")\n");
            }
        }
        
        // Add default values if not provided
        if (!has_title) {
            print_indent(out, indent);
            fprintf(out, "plt.title('Timeline')\n");
        }
        if (!has_xlabel) {
            print_indent(out, indent);
            fprintf(out, "plt.xlabel('Date')\n");
        }
        if (!has_ylabel) {
            print_indent(out, indent);
            fprintf(out, "plt.ylabel('Value')\n");
        }
        if (!has_grid) {
            print_indent(out, indent);
            fprintf(out, "plt.grid(True)\n");
        }
        if (!has_autofmt_xdate) {
            print_indent(out, indent);
            fprintf(out, "plt.gcf().autofmt_xdate()\n");
        }
        
        print_indent(out, indent);
        fprintf(out, "plt.show()\n");
    } else {
        fprintf(out, "# Unknown visualization: %s\n", func);
    }
}

void generate_expr(ASTNode* node, FILE* out, int indent) {
    if (!node) return;
    switch (node->type) {
        case NODE_NUMBER:
            fprintf(out, "%.15g", node->num_value);
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