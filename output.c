#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    LispValue* my_cycle = NULL;
    lisp_print(lisp_make_string("=== 1. Init ==="));
    gc_print_stats();
    my_cycle = lisp_cons(lisp_make_int(1), NULL);
    lisp_set_cdr(my_cycle, my_cycle);
    lisp_print(lisp_make_string("=== 2. Cycle created ==="));
    gc_print_stats();
    lisp_release(my_cycle);
    lisp_print(lisp_make_string("=== 3. Reference dropped ==="));
    gc_print_stats();
    gc_collect_cycles();
    lisp_print(lisp_make_string("=== 4. After Mark-and-Sweep ==="));
    gc_print_stats();

    /* Cleanup: release all variables */
    lisp_release(my_cycle);
    gc_shutdown();
    return 0;
}