#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* Forward declarations */
LispValue* make_adder(LispValue* n);

LispValue* __lambda_0(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    LispValue* n = lisp_env_lookup(__env, "n");
    return lisp_add(x, n);
}

LispValue* make_adder(LispValue* n) {
    return lisp_make_closure(__lambda_0, lisp_env_extend(NULL, "n", n));
}

LispValue* make_adder_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* n = lisp_list_get(__args, 0);
    return make_adder(n);
}

int main(int argc, char** argv) {
    LispValue* add5 = NULL;
    add5 = make_adder(lisp_make_int(5));
    lisp_print(lisp_call_closure(add5, lisp_cons(lisp_make_int(10), NULL)));
    return 0;
}