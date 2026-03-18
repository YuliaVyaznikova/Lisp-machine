#include <stdio.h>
#include <stdlib.h>
#include "runtime/lisp.h"

/* Forward declarations */
LispValue* map(LispValue* f, LispValue* lst);
LispValue* filter(LispValue* pred, LispValue* lst);
LispValue* reduce(LispValue* f, LispValue* init, LispValue* lst);
LispValue* zero_p(LispValue* x);
LispValue* positive_p(LispValue* x);
LispValue* negative_p(LispValue* x);
LispValue* even_p(LispValue* x);
LispValue* odd_p(LispValue* x);
LispValue* inc(LispValue* x);
LispValue* dec(LispValue* x);
LispValue* square(LispValue* x);
LispValue* cube(LispValue* x);

LispValue* map(LispValue* f, LispValue* lst) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(lst))) {
        __result_0 = NULL;
    } else {
        __result_0 = lisp_cons(lisp_call_closure(f, lisp_cons(lisp_first(lst), NULL)), map(f, lisp_rest(lst)));
    }
    return __result_0;
}

LispValue* map_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return map(f, lst);
}

LispValue* filter(LispValue* pred, LispValue* lst) {
    LispValue* __result_0 = NULL;
    if (lisp_is_true(lisp_is_nil_fn(lst))) {
        __result_0 = NULL;
    } else {
        LispValue* __result_1 = NULL;
        if (lisp_is_true(lisp_call_closure(pred, lisp_cons(lisp_first(lst), NULL)))) {
            __result_1 = lisp_cons(lisp_first(lst), filter(pred, lisp_rest(lst)));
        } else {
            __result_1 = filter(pred, lisp_rest(lst));
        }
        __result_0 = __result_1;
    }
    return __result_0;
}

LispValue* filter_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* pred = lisp_list_get(__args, 0);
    LispValue* lst = lisp_list_get(__args, 1);
    return filter(pred, lst);
}

LispValue* reduce(LispValue* f, LispValue* init, LispValue* lst) {
    LispValue* __new_f = NULL;
    LispValue* __new_init = NULL;
    LispValue* __new_lst = NULL;
    while (1) {
        if (lisp_is_true(lisp_is_nil_fn(lst))) {
            return init;
        } else {
            __new_f = f;
            __new_init = lisp_call_closure(f, lisp_cons(init, lisp_cons(lisp_first(lst), NULL)));
            __new_lst = lisp_rest(lst);
            f = __new_f;
            init = __new_init;
            lst = __new_lst;
            continue;
        }
    }
}

LispValue* reduce_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* f = lisp_list_get(__args, 0);
    LispValue* init = lisp_list_get(__args, 1);
    LispValue* lst = lisp_list_get(__args, 2);
    return reduce(f, init, lst);
}

LispValue* zero_p(LispValue* x) {
    return lisp_eq(x, lisp_make_int(0));
}

LispValue* zero_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return zero_p(x);
}

LispValue* positive_p(LispValue* x) {
    return lisp_gt(x, lisp_make_int(0));
}

LispValue* positive_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return positive_p(x);
}

LispValue* negative_p(LispValue* x) {
    return lisp_lt(x, lisp_make_int(0));
}

LispValue* negative_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return negative_p(x);
}

LispValue* even_p(LispValue* x) {
    return lisp_eq(lisp_mod(x, lisp_make_int(2)), lisp_make_int(0));
}

LispValue* even_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return even_p(x);
}

LispValue* odd_p(LispValue* x) {
    return lisp_not(even_p(x));
}

LispValue* odd_p_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return odd_p(x);
}

LispValue* inc(LispValue* x) {
    return lisp_add(x, lisp_make_int(1));
}

LispValue* inc_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return inc(x);
}

LispValue* dec(LispValue* x) {
    return lisp_sub(x, lisp_make_int(1));
}

LispValue* dec_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return dec(x);
}

LispValue* square(LispValue* x) {
    return lisp_mul(x, x);
}

LispValue* square_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return square(x);
}

LispValue* cube(LispValue* x) {
    return lisp_mul(lisp_mul(x, x), x);
}

LispValue* cube_wrapper(LispValue* __args, LispValue* __env) {
    LispValue* x = lisp_list_get(__args, 0);
    return cube(x);
}

int main(int argc, char** argv) {
    lisp_print(map(lisp_make_closure(inc_wrapper, NULL), lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), lisp_cons(lisp_make_int(4), lisp_cons(lisp_make_int(5), NULL)))))));
    lisp_print(filter(lisp_make_closure(even_p_wrapper, NULL), lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), lisp_cons(lisp_make_int(4), lisp_cons(lisp_make_int(5), lisp_cons(lisp_make_int(6), lisp_cons(lisp_make_int(7), lisp_cons(lisp_make_int(8), NULL))))))))));
    lisp_print(reduce(lisp_make_closure(lisp_add_wrapper, NULL), lisp_make_int(0), lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), lisp_cons(lisp_make_int(3), lisp_cons(lisp_make_int(4), lisp_cons(lisp_make_int(5), NULL)))))));
    return 0;
}