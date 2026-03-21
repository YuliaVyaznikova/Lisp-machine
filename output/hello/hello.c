#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    lisp_print(lisp_make_string("Hello, World!"));

    /* Cleanup: release all variables */
    gc_shutdown();
    return 0;
}