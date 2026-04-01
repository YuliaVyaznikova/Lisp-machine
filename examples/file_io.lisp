(print "=== File I/O test ===")

(print "Writing to file:")
(define f (open "test_output.txt" "w"))
(file-write-line f "Hello from Lisp!")
(file-write-line f "Line 2")
(file-write-line f "Line 3")
(close f)
(print "File written")

(print "Reading from file:")
(define f2 (open "test_output.txt" "r"))
(print (file-read-line f2))
(print (file-read-line f2))
(print (file-read-line f2))
(print (file-eof f2))
(close f2)

(print "=== Done ===")