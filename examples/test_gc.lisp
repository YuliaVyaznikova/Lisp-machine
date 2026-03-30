(print "=== GC Demonstration ===")
(print "")
(print "1. Reference counting frees trees automatically")
(gc-stats)

(define root (cons (cons 1 2) (cons 3 4)))
(print "Tree: ((1.2).(3.4))")
(gc-stats)

(drop root)
(print "Dropped root - tree freed recursively:")
(gc-stats)

(print "")
(print "2. Cycles need mark-and-sweep")
(define cycle (cons 1 nil))
(set-cdr! cycle cycle)
(print "Self-referential cycle created")
(gc-stats)

(drop cycle)
(print "Dropped - but cycle has internal ref:")
(gc-stats)

(gc-collect)
(print "After mark-and-sweep:")
(gc-stats)

(print "")
(print "=== Done ===")