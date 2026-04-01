(print "=== c-emit test ===")

(print "Calling C printf directly:")
(c-emit "printf(\"Hello from C!\\n\")")

(print "Calling C math:")
(c-emit "printf(\"Square root of 2: %f\\n\", 1.41421356 * 1.41421356)")

(print "Lisp value:")
(print (+ 1 2))

(print "=== Done ===")