#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_eq(lisp_make_int(5), lisp_make_int(5)));
    lisp_print(lisp_lt(lisp_make_int(3), lisp_make_int(10)));
    lisp_print(lisp_gt(lisp_make_int(10), lisp_make_int(3)));
    lisp_print(lisp_eq(lisp_make_int(1), lisp_make_int(2)));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}