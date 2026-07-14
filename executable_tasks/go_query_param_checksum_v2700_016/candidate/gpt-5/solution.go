package main

func run(rows []string) uint64 {
	const offset64 = 1469598103934665603
	const prime64 = 1099511628211

	hash := uint64(offset64)
	for _, s := range rows {
		// Parse u
		i := 0
		n := len(s)
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++ // skip '='
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
		if i < n && s[i] == '&' {
			i++
		}

		// Parse score
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++ // skip '='
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
		if i < n && s[i] == '&' {
			i++
		}

		// Parse flag
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++ // skip '='
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

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
