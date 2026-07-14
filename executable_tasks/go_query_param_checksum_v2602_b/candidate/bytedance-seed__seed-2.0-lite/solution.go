package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		b := []byte(row)
		var amp1, amp2 int
		// Locate the two mandatory & separators for fixed-order parameters
		for i, c := range b {
			if c == '&' {
				if amp1 == 0 {
					amp1 = i
				} else {
					amp2 = i
					break
				}
			}
		}
		// Parse u from after "u=" to first &
		var u int
		for i := 2; i < amp1; i++ {
			u = u*10 + int(b[i]-'0')
		}
		// Parse score from after "score=" (6 bytes) to second &
		var score int
		for i := amp1 + 7; i < amp2; i++ {
			score = score*10 + int(b[i]-'0')
		}
		// Parse flag from after "flag=" (5 bytes) to end of string
		var flag int
		for i := amp2 + 6; i < len(b); i++ {
			flag = flag*10 + int(b[i]-'0')
		}
		// Preserve exact hash update logic from original implementation
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}