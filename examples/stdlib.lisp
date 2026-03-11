(define (map f lst)
  (if (nil? lst)
      nil
      (cons (f (first lst)) (map f (rest lst)))))

(define (filter pred lst)
  (if (nil? lst)
      nil
      (if (pred (first lst))
          (cons (first lst) (filter pred (rest lst)))
          (filter pred (rest lst)))))

(define (reduce f init lst)
  (if (nil? lst)
      init
      (reduce f (f init (first lst)) (rest lst))))

(define (zero? x) (= x 0))

(define (positive? x) (> x 0))

(define (negative? x) (< x 0))

(define (even? x) (= (mod x 2) 0))

(define (odd? x) (not (even? x)))

(define (inc x) (+ x 1))

(define (dec x) (- x 1))

(define (square x) (* x x))

(define (cube x) (* x x x))

(print (map inc '(1 2 3 4 5)))
(print (filter even? '(1 2 3 4 5 6 7 8)))
(print (reduce + 0 '(1 2 3 4 5)))