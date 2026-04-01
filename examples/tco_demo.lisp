(print "=== TCO Demonstration ===")
(print "")

(print "Without TCO, this would overflow the stack:")
(print "Counting down from 100000...")

(define (count-down n)
  (if (= n 0)
      "done!"
      (count-down (- n 1))))

(print (count-down 100000))

(print "")
(print "=== Done ===")
