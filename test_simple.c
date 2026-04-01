#include <stdio.h>
#include "runtime/lisp.h"

LispValue* sum_all(LispValue* nums);
LispValue* sum_all_wrapper(LispValue* __args, LispValue* __env);

LispValue* sum_all(LispValue* nums) {
    printf("sum_all called with nums=%p\n", (void*)nums);
    if (nums == NULL || nums->type == LISP_NIL) {
        printf("  nums is nil, returning 0\n");
        return lisp_make_int(0);
    }
    printf("  nums is a pair, first=");
    LispValue* first = lisp_first(nums);
    if (first) printf("%ld", (long)first->data.int_val);
    else printf("NULL");
    printf(", rest=%p\n", (void*)lisp_rest(nums));
    
    LispValue* rest = lisp_rest(nums);
    LispValue* rest_sum = sum_all_wrapper(rest, NULL);
    if (first && rest_sum) {
        return lisp_add(first, rest_sum);
    }
    return lisp_make_int(0);
}

LispValue* sum_all_wrapper(LispValue* __args, LispValue* __env) {
    printf("wrapper called with __args=%p\n", (void*)__args);
    return sum_all(__args);
}

int main() {
    gc_init();
    printf("Test 1: sum_all()\n");
    lisp_print(sum_all_wrapper(NULL, NULL));
    printf("\nTest 2: sum_all(1)\n");
    lisp_print(sum_all_wrapper(lisp_cons(lisp_make_int(1), NULL), NULL));
    printf("\nTest 3: sum_all(1,2)\n");
    lisp_print(sum_all_wrapper(lisp_cons(lisp_make_int(1), lisp_cons(lisp_make_int(2), NULL)), NULL));
    gc_shutdown();
    return 0;
}
