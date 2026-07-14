package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, s := range rows {
		// Parse u after "u="
		i := 2
		neg := false
		if i < len(s) {
			if s[i] == '-' {
				neg = true
				i++
			} else if s[i] == '+' {
				i++
			}
		}
		n := 0
		for i < len(s) {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			n = n*10 + int(c-'0')
			i++
		}
		if neg {
			n = -n
		}
		u := n

		// Skip '&' and "score=" (6 chars)
		i++
		i += 6

		// Parse score
		neg = false
		if i < len(s) {
			if s[i] == '-' {
				neg = true
				i++
			} else if s[i] == '+' {
				i++
			}
		}
		n = 0
		for i < len(s) {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			n = n*10 + int(c-'0')
			i++
		}
		if neg {
			n = -n
		}
		score := n

		// Skip '&' and "flag=" (5 chars)
		i++
		i += 5

		// Parse flag
		neg = false
		if i < len(s) {
			if s[i] == '-' {
				neg = true
				i++
			} else if s[i] == '+' {
				i++
			}
		}
		n = 0
		for i < len(s) {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			n = n*10 + int(c-'0')
			i++
		}
		if neg {
			n = -n
		}
		flag := n

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
