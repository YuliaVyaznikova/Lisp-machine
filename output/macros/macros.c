#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_string("=== when macro ==="));
    lisp_print(NULL);
    lisp_print(NULL);
    lisp_print(lisp_make_string("=== unless macro ==="));
    lisp_print(NULL);
    lisp_print(NULL);
    lisp_print(lisp_make_string("=== let macro ==="));
    lisp_print(NULL);
    lisp_print(NULL);
    lisp_print(lisp_make_string("=== swap macro ==="));
    lisp_print(NULL);
    lisp_print(lisp_make_string("=== inc macro ==="));
    lisp_print(NULL);
    lisp_print(NULL);
    lisp_print(lisp_make_string("=== dec macro ==="));
    lisp_print(NULL);
    lisp_print(NULL);
    lisp_print(lisp_make_string("=== assert macro ==="));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}