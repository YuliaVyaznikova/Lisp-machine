(print "=== I/O Test ===")

(print "princ - output without quotes:")
(princ "Hello, World!")
(terpri)

(princ "Number: ")
(princ 42)
(terpri)

(princ "Symbol: ")
(princ 'foo)
(terpri)

(print "")
(print "terpri - newline:")
(princ "Line 1")
(terpri)
(princ "Line 2")
(terpri)

(print "")
(print "read-line - read a line:")
(princ "Enter your name: ")
(define name (read-line))
(princ "Hello, ")
(princ name)
(terpri)

(print "")
(print "read-char - read a character:")
(princ "Enter a char: ")
(define ch (read-char))
(princ "You entered: ")
(princ ch)
(terpri)

(print "")
(print "=== Done ===")