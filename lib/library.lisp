(define (zero? x) (= x 0))

(define (positive? x) (> x 0))

(define (negative? x) (< x 0))

(define (even? x) (= (mod x 2) 0))

(define (odd? x) (not (even? x)))

(define (inc x) (+ x 1))

(define (dec x) (- x 1))

(define (square x) (* x x))

(define (cube x) (* x x x))

(define (length lst)
  (if (nil? lst)
      0
      (+ 1 (length (rest lst)))))

(define (append lst1 lst2)
  (if (nil? lst1)
      lst2
      (cons (first lst1) (append (rest lst1) lst2))))

(define (reverse lst)
  (if (nil? lst)
      nil
      (append (reverse (rest lst)) (cons (first lst) nil))))

(define (nth n lst)
  (if (= n 0)
      (first lst)
      (nth (- n 1) (rest lst))))

(define (last lst)
  (if (nil? (rest lst))
      (first lst)
      (last (rest lst))))

(define (member x lst)
  (if (nil? lst)
      nil
      (if (= x (first lst))
          lst
          (member x (rest lst)))))

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

(define (any? pred lst)
  (if (nil? lst)
      nil
      (if (pred (first lst))
          (first lst)
          (any? pred (rest lst)))))

(define (all? pred lst)
  (if (nil? lst)
      1
      (if (pred (first lst))
          (all? pred (rest lst))
          nil)))

(defmacro when (condition body)
  `(if ,condition ,body nil))

(defmacro unless (condition body)
  `(if ,condition nil ,body))

(defmacro let (var value body)
  `((lambda (,var) ,body) ,value))

(define (identity x) x)

(define (constantly x)
  (lambda (y) x))

(define (compose f g)
  (lambda (x) (f (g x))))

(define (flip f)
  (lambda (x y) (f y x)))

(define (curry f x)
  (lambda (y) (f x y)))