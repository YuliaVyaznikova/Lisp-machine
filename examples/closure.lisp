(define (make-adder n) (lambda (x) (+ x n)))
(define add5 (make-adder 5))
(print (add5 10))
