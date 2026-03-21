#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_symbol("foo"));
    lisp_print(lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), NULL))));
    lisp_print(NULL);

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}