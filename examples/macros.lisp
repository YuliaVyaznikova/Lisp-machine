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
  `(if ,condition
       (print (cons "PASS:" ,message))
       (print (cons "FAIL:" ,message))))

(print "=== assert macro ===")
(assert (= 1 1) "1 equals 1")
(assert (< 1 2) "1 is less than 2")