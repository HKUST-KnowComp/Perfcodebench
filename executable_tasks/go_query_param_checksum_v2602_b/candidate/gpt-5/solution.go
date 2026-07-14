package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, s := range rows {
		// Parse u
		i := 2 // skip "u="
		neg := false
		if i < len(s) && s[i] == '-' {
			neg = true
			i++
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
		if neg {
			u = -u
		}

		// Skip '&' and "score="
		if i < len(s) && s[i] == '&' {
			i++
		}
		i += 6 // len("score=")

		// Parse score
		neg = false
		if i < len(s) && s[i] == '-' {
			neg = true
			i++
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
		if neg {
			score = -score
		}

		// Skip '&' and "flag="
		if i < len(s) && s[i] == '&' {
			i++
		}
		i += 5 // len("flag=")

		// Parse flag
		neg = false
		if i < len(s) && s[i] == '-' {
			neg = true
			i++
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
		if neg {
			flag = -flag
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
