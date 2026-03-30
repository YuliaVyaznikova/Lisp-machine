(print "=== 1. Init ===")
(gc-stats)

(define my-cycle (cons 1 nil))

(set-cdr! my-cycle my-cycle)

(print "=== 2. Cycle created ===")
(gc-stats)

(drop my-cycle)

(print "=== 3. Reference dropped ===")
(gc-stats)

(gc-collect)

(print "=== 4. After Mark-and-Sweep ===")
(gc-stats)