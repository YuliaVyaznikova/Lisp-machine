(define (even? n) (if (= n 0) true (odd? (- n 1)))) (define (odd? n) (if (= n 0) false (even? (- n 1)))) (print (even? 10)) (print (odd? 10)) (print (even? 7)) (print (odd? 7))
