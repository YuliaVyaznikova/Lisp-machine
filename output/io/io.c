#include <stdio.h>
#include "runtime/lisp.h"

/* GC initialization is done in main() */

/* Forward declarations */



int main(int argc, char** argv) {
    gc_init();

    LispValue* name = NULL;
    LispValue* ch = NULL;
    lisp_print(lisp_make_string("=== I/O Test ==="));
    lisp_print(lisp_make_string("princ - output without quotes:"));
    lisp_princ(lisp_make_string("Hello, World!"));
    lisp_terpri_wrapper(NULL, NULL);
    lisp_princ(lisp_make_string("Number: "));
    lisp_princ(lisp_make_int(42));
    lisp_terpri_wrapper(NULL, NULL);
    lisp_princ(lisp_make_string("Symbol: "));
    lisp_princ(lisp_make_symbol("foo"));
    lisp_terpri_wrapper(NULL, NULL);
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("terpri - newline:"));
    lisp_princ(lisp_make_string("Line 1"));
    lisp_terpri_wrapper(NULL, NULL);
    lisp_princ(lisp_make_string("Line 2"));
    lisp_terpri_wrapper(NULL, NULL);
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("read-line - read a line:"));
    lisp_princ(lisp_make_string("Enter your name: "));
    name = lisp_read_line_wrapper(NULL, NULL);
    lisp_princ(lisp_make_string("Hello, "));
    lisp_princ(name);
    lisp_terpri_wrapper(NULL, NULL);
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("read-char - read a character:"));
    lisp_princ(lisp_make_string("Enter a char: "));
    ch = lisp_read_char_wrapper(NULL, NULL);
    lisp_princ(lisp_make_string("You entered: "));
    lisp_princ(ch);
    lisp_terpri_wrapper(NULL, NULL);
    lisp_print(lisp_make_string(""));
    lisp_print(lisp_make_string("=== Done ==="));

    /* Cleanup: release all variables */
    lisp_release(ch);
    lisp_release(name);
    gc_shutdown();
    return 0;
}