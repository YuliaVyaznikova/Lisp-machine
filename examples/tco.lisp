; tail call optimization examples

; factorial with accumulator
(define (factorial-acc n acc)
  (if (= n 0)
      acc
      (factorial-acc (- n 1) (* n acc))))

(define (factorial n)
  (factorial-acc n 1))

(print (factorial 5))
(print (factorial 10))

; sum from 1 to n
(define (sum-to-acc n acc)
  (if (= n 0)
      acc
      (sum-to-acc (- n 1) (+ n acc))))

(define (sum-to n)
  (sum-to-acc n 0))

(print (sum-to 100))

; fibonacci
(define (fib-acc n a b)
  (if (= n 0)
      a
      (fib-acc (- n 1) b (+ a b))))

(define (fib n)
  (fib-acc n 0 1))

(print (fib 10))
(print (fib 20))