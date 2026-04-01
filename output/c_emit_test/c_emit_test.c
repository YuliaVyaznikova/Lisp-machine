#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_string("=== c-emit test ==="));
    lisp_print(lisp_make_string("Calling C printf directly:"));
    (printf("Hello from C!\n"));
    lisp_print(lisp_make_string("Calling C math:"));
    (printf("Square root of 2: %f\n", 1.41421356 * 1.41421356));
    lisp_print(lisp_make_string("Lisp value:"));
    lisp_print(lisp_add(lisp_make_int(1), lisp_make_int(2)));
    lisp_print(lisp_make_string("=== Done ==="));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}