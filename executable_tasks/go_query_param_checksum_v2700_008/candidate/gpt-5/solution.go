package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, s := range rows {
		n := len(s)
		i := 2 // skip "u="

		// parse u
		neg := false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		u := 0
		for i < n {
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
		// skip '&'
		if i < n && s[i] == '&' {
			i++
		}

		// skip "score=" (6 chars)
		i += 6

		// parse score
		neg = false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		score := 0
		for i < n {
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
		// skip '&'
		if i < n && s[i] == '&' {
			i++
		}

		// skip "flag=" (5 chars)
		i += 5

		// parse flag
		neg = false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		flag := 0
		for i < n {
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
