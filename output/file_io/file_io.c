#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    LispValue* f = NULL;
    LispValue* f2 = NULL;
    lisp_print(lisp_make_string("=== File I/O test ==="));
    lisp_print(lisp_make_string("Writing to file:"));
    f = lisp_open_wrapper(lisp_cons(lisp_make_string("test_output.txt"), lisp_cons(lisp_make_string("w"), NULL)), NULL);
    lisp_file_write_line_wrapper(lisp_cons(f, lisp_cons(lisp_make_string("Hello from Lisp!"), NULL)), NULL);
    lisp_file_write_line_wrapper(lisp_cons(f, lisp_cons(lisp_make_string("Line 2"), NULL)), NULL);
    lisp_file_write_line_wrapper(lisp_cons(f, lisp_cons(lisp_make_string("Line 3"), NULL)), NULL);
    lisp_close_wrapper(lisp_cons(f, NULL), NULL);
    lisp_print(lisp_make_string("File written"));
    lisp_print(lisp_make_string("Reading from file:"));
    f2 = lisp_open_wrapper(lisp_cons(lisp_make_string("test_output.txt"), lisp_cons(lisp_make_string("r"), NULL)), NULL);
    lisp_print(lisp_file_read_line_wrapper(lisp_cons(f2, NULL), NULL));
    lisp_print(lisp_file_read_line_wrapper(lisp_cons(f2, NULL), NULL));
    lisp_print(lisp_file_read_line_wrapper(lisp_cons(f2, NULL), NULL));
    lisp_print(lisp_file_eof_wrapper(lisp_cons(f2, NULL), NULL));
    lisp_close_wrapper(lisp_cons(f2, NULL), NULL);
    lisp_print(lisp_make_string("=== Done ==="));

    /* Cleanup: release all variables */
    lisp_release(f2);
    lisp_release(f);
    gc_shutdown();
    return 0;
}