package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, s := range rows {
		i := 0

		// Parse u
		for i < len(s) && s[i] != '=' {
			i++
		}
		if i < len(s) {
			i++
		}
		sign := 1
		if i < len(s) {
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
		}
		u := 0
		for i < len(s) {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + int(c-'0')
			i++
		}
		u *= sign

		// Parse score
		for i < len(s) && s[i] != '=' {
			i++
		}
		if i < len(s) {
			i++
		}
		sign = 1
		if i < len(s) {
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
		}
		score := 0
		for i < len(s) {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + int(c-'0')
			i++
		}
		score *= sign

		// Parse flag
		for i < len(s) && s[i] != '=' {
			i++
		}
		if i < len(s) {
			i++
		}
		sign = 1
		if i < len(s) {
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
		}
		flag := 0
		for i < len(s) {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			flag = flag*10 + int(c-'0')
			i++
		}
		flag *= sign

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
