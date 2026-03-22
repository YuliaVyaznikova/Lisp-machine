; when - execute body only if condition is true
(defmacro when (condition body)
  `(if ,condition ,body nil))

(print "=== when macro ===")
(print (when (= 1 1) "true branch"))
(print (when (= 1 2) "should not print"))

; unless - execute body only if condition is false
(defmacro unless (condition body)
  `(if ,condition nil ,body))

(print "=== unless macro ===")
(print (unless (= 1 2) "condition was false"))
(print (unless (= 1 1) "should not print"))

; cond - multi-branch conditional (like switch/case)
; Returns the body of the first true condition
; (cond (test1 body1) (test2 body2) (else body3))
(defmacro cond (pair1 pair2 pair3)
  `(if ,(first pair1)
       ,(first (rest pair1))
       (if ,(first pair2)
           ,(first (rest pair2))
           (if ,(first pair3)
               ,(first (rest pair3))
               nil))))

(print "=== cond macro ===")
(print (cond ((= 1 2) "one equals two")
             ((= 1 1) "one equals one")
             (true "else branch")))
(print (cond ((= 1 2) "first")
             ((= 2 3) "second")
             (true "fallback")))

; and - logical and with short-circuit evaluation
(defmacro and (a b)
  `(if ,a ,b nil))

(print "=== and macro ===")
(print (and true true))
(print (and true false))
(print (and false true))
(print (and (= 1 1) (= 2 2)))

; or - logical or with short-circuit evaluation
(defmacro or (a b)
  `(if ,a true ,b))

(print "=== or macro ===")
(print (or true false))
(print (or false true))
(print (or false false))
(print (or (= 1 2) (= 2 2)))

; let - local variable binding
(defmacro let (var value body)
  `((lambda (,var) ,body) ,value))

(print "=== let macro ===")
(print (let x 10 (+ x 5)))
(print (let y 20 (* y 2)))

; swap - swap two values in a list
(defmacro swap (a b)
  `(cons ,b (cons ,a nil)))

(print "=== swap macro ===")
(print (swap 1 2))

; inc - increment a value
(defmacro inc (n)
  `(+ ,n 1))

(print "=== inc macro ===")
(print (inc 5))
(print (inc (inc 10)))

; dec - decrement a value
(defmacro dec (n)
  `(- ,n 1))

(print "=== dec macro ===")
(print (dec 5))
(print (dec (dec 10)))

; assert - simple assertion macro
(defmacro assert (condition message)
  `(print (if ,condition
               (cons "PASS:" ,message)
               (cons "FAIL:" ,message))))

(print "=== assert macro ===")
(assert (= 1 1) "1 equals 1")
(assert (< 1 2) "1 is less than 2")

; not-nil? - check if value is not nil
(defmacro not-nil? (x)
  `(if ,x true false))

(print "=== not-nil? macro ===")
(print (not-nil? 1))
(print (not-nil? nil))

; forever - infinite loop (use with caution!)
; (defmacro forever (body)
;   `(while true ,body))