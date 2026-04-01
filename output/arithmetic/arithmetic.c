#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_add(lisp_add(lisp_make_int(1), lisp_make_int(2)), lisp_make_int(3)));
    lisp_print(lisp_mul(lisp_make_int(6), lisp_make_int(7)));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}