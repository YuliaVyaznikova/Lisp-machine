(print "=== &rest parameter test ===")

(print "sum-all with &rest:")
(define (sum-all &rest nums)
  (if (nil? nums)
      0
      (+ (first nums) (apply sum-all (rest nums)))))

(print (sum-all))
(print (sum-all 1))
(print (sum-all 1 2))
(print (sum-all 1 2 3))
(print (sum-all 1 2 3 4 5))

(print "=== Done ===")