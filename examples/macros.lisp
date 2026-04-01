(defmacro when (condition body)
  `(if ,condition ,body nil))

(print "=== when macro ===")
(print (when (= 1 1) "true branch"))
(print (when (= 1 2) "should not print"))

(defmacro unless (condition body)
  `(if ,condition nil ,body))

(print "=== unless macro ===")
(print (unless (= 1 2) "condition was false"))
(print (unless (= 1 1) "should not print"))

(defmacro cond (&rest clauses)
  (if (nil? clauses)
      nil
      `(if ,(first (first clauses))
           ,(first (rest (first clauses)))
           (cond ,@(rest clauses)))))

(print "=== cond macro ===")
(print "cond with 1 clause:")
(print (cond (true "only one")))
(print "cond with 2 clauses:")
(print (cond ((= 1 2) "first")
             (true "second")))
(print "cond with 3 clauses:")
(print (cond ((= 1 2) "one equals two")
             ((= 1 1) "one equals one")
             (true "else branch")))
(print "cond with 5 clauses:")
(print (cond ((= 1 5) "one is five")
             ((= 1 4) "one is four")
             ((= 1 3) "one is three")
             ((= 1 2) "one is two")
             (true "one is one")))
(print "cond with 7 clauses:")
(print (cond ((= 1 7) "1")
             ((= 1 6) "2")
             ((= 1 5) "3")
             ((= 1 4) "4")
             ((= 1 3) "5")
             ((= 1 2) "6")
             (true "matched at 7th clause")))

(defmacro and (a b)
  `(if ,a ,b nil))

(print "=== and macro ===")
(print (and true true))
(print (and true false))
(print (and false true))
(print (and (= 1 1) (= 2 2)))

(defmacro or (a b)
  `(if ,a true ,b))

(print "=== or macro ===")
(print (or true false))
(print (or false true))
(print (or false false))
(print (or (= 1 2) (= 2 2)))

(defmacro let (var value body)
  `((lambda (,var) ,body) ,value))

(print "=== let macro ===")
(print (let x 10 (+ x 5)))
(print (let y 20 (* y 2)))

(defmacro swap (a b)
  `(cons ,b (cons ,a nil)))

(print "=== swap macro ===")
(print (swap 1 2))

(defmacro inc (n)
  `(+ ,n 1))

(print "=== inc macro ===")
(print (inc 5))
(print (inc (inc 10)))

(defmacro dec (n)
  `(- ,n 1))

(print "=== dec macro ===")
(print (dec 5))
(print (dec (dec 10)))

(defmacro assert (condition message)
  `(print (if ,condition
               (cons "PASS:" ,message)
               (cons "FAIL:" ,message))))

(print "=== assert macro ===")
(assert (= 1 1) "1 equals 1")
(assert (< 1 2) "1 is less than 2")

(defmacro not-nil? (x)
  `(if ,x true false))

(print "=== not-nil? macro ===")
(print (not-nil? 1))
(print (not-nil? nil))