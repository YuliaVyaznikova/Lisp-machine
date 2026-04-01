#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    LispValue* root = NULL;
    LispValue* cycle = NULL;
    lisp_print(lisp_make_string("=== GC Demonstration ==="));
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("1. Reference counting frees trees automatically"));
    gc_print_stats();
    root = lisp_cons(lisp_cons(lisp_make_int(1), lisp_make_int(2)), lisp_cons(lisp_make_int(3), lisp_make_int(4)));
    lisp_print(lisp_make_string("Tree: ((1.2).(3.4))"));
    gc_print_stats();
    lisp_release(root);
    lisp_print(lisp_make_string("Dropped root - tree freed recursively:"));
    gc_print_stats();
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("2. Cycles need mark-and-sweep"));
    cycle = lisp_cons(lisp_make_int(1), NULL);
    lisp_set_cdr(cycle, cycle);
    lisp_print(lisp_make_string("Self-referential cycle created"));
    gc_print_stats();
    lisp_release(cycle);
    lisp_print(lisp_make_string("Dropped - but cycle has internal ref:"));
    gc_print_stats();
    gc_collect_cycles();
    lisp_print(lisp_make_string("After mark-and-sweep:"));
    gc_print_stats();
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("=== Done ==="));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}