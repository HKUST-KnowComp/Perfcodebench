package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, s := range rows {
		n := len(s)
		// Parse u (after "u=")
		i := 2
		u := 0
		neg := false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		for i < n {
			c := s[i]
			if c == '&' {
				break
			}
			u = u*10 + int(c-'0')
			i++
		}
		if neg {
			u = -u
		}

		// Skip '&'
		i++
		// Skip "score=" (6 chars)
		i += 6
		// Parse score
		score := 0
		neg = false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		for i < n {
			c := s[i]
			if c == '&' {
				break
			}
			score = score*10 + int(c-'0')
			i++
		}
		if neg {
			score = -score
		}

		// Skip '&'
		i++
		// Skip "flag=" (5 chars)
		i += 5
		// Parse flag (until end)
		flag := 0
		neg = false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		for i < n {
			c := s[i]
			if c == '&' {
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
		hash *= prime
	}
	return hash
}
