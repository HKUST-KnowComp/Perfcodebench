package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		idx := 0
		// skip "u="
		idx += 2
		u, idx := atoi(row, idx)
		// skip "&score=" (1 '&' + 6 "score=")
		idx += 7
		score, idx := atoi(row, idx)
		// skip "&flag=" (1 '&' + 5 "flag=")
		idx += 6
		flag, idx := atoi(row, idx)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// atoi parses a non-negative integer starting at index i in s.
// It returns the integer value and the index after the last digit.
func atoi(s string, i int) (int, int) {
	val := 0
	for i < len(s) && s[i] >= '0' && s[i] <= '9' {
		val = val*10 + int(s[i]-'0')
		i++
	}
	return val, i
}