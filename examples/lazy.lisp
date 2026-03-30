(defmacro lazy-cons (a b)
  `(cons ,a (lambda () ,b)))

(define (lazy-first s) (first s))
(define (lazy-rest s) ((rest s)))

(define (lazy-map f s)
  (if (nil? s)
      nil
      (lazy-cons (f (lazy-first s)) (lazy-map f (lazy-rest s)))))

(define (lazy-filter pred s)
  (if (nil? s)
      nil
      (if (pred (lazy-first s))
          (lazy-cons (lazy-first s) (lazy-filter pred (lazy-rest s)))
          (lazy-filter pred (lazy-rest s)))))

(define (take n s)
  (if (= n 0)
      nil
      (if (nil? s)
          nil
          (lazy-cons (lazy-first s) (take (- n 1) (lazy-rest s))))))

(define (force-list s)
  (if (nil? s)
      nil
      (cons (lazy-first s) (force-list (lazy-rest s)))))

(define (integers-from n)
  (lazy-cons n (integers-from (+ n 1))))

(define (even? x) (= (mod x 2) 0))
(define (square x) (* x x))

(define ints (integers-from 1))
(define evens (lazy-filter even? ints))
(define squares-of-evens (lazy-map square evens))

(print (force-list (take 5 squares-of-evens)))