(print "=== Mutual recursion test ===")
(print "foo calls bar, bar defined AFTER foo:")

(define (foo x) (bar x))
(define (bar y) y)

(print (foo 42))
(print "Expected: 42")

(print "")
(print "=== even? / odd? mutual recursion ===")

(define (even? n) (if (= n 0) true (odd? (- n 1))))
(define (odd? n) (if (= n 0) false (even? (- n 1))))

(print "even? 10:")
(print (even? 10))
(print "odd? 10:")
(print (odd? 10))
(print "even? 7:")
(print (even? 7))
(print "odd? 7:")
(print (odd? 7))