package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, s := range rows {
		// Parse u
		i := 2 // skip "u="
		n := len(s)
		var u int64
		neg := false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		for i < n {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + int64(c-'0')
			i++
		}
		if neg {
			u = -u
		}

		// skip '&'
		if i < n && s[i] == '&' {
			i++
		}

		// Parse score (skip "score=")
		i += 6
		var score int64
		neg = false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		for i < n {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + int64(c-'0')
			i++
		}
		if neg {
			score = -score
		}

		// skip '&'
		if i < n && s[i] == '&' {
			i++
		}

		// Parse flag (skip "flag=")
		i += 5
		var flag int64
		neg = false
		if i < n && s[i] == '-' {
			neg = true
			i++
		}
		for i < n {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			flag = flag*10 + int64(c-'0')
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
