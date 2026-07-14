package main

func run(rows []string) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	var hash uint64 = offset
	for _, s := range rows {
		n := len(s)
		i := 2 // skip "u="

		// parse u
		neg := false
		if i < n {
			c := s[i]
			if c == '-' {
				neg = true
				i++
			} else if c == '+' {
				i++
			}
		}
		u := 0
		for i < n {
			c := s[i]
			d := int(c - '0')
			if d < 0 || d > 9 {
				break
			}
			u = u*10 + d
			i++
		}
		if neg {
			u = -u
		}
		if i < n && s[i] == '&' {
			i++
		}

		// parse score ("score=" is 6 chars)
		i += 6
		neg = false
		if i < n {
			c := s[i]
			if c == '-' {
				neg = true
				i++
			} else if c == '+' {
				i++
			}
		}
		score := 0
		for i < n {
			c := s[i]
			d := int(c - '0')
			if d < 0 || d > 9 {
				break
			}
			score = score*10 + d
			i++
		}
		if neg {
			score = -score
		}
		if i < n && s[i] == '&' {
			i++
		}

		// parse flag ("flag=" is 5 chars)
		i += 5
		neg = false
		if i < n {
			c := s[i]
			if c == '-' {
				neg = true
				i++
			} else if c == '+' {
				i++
			}
		}
		flag := 0
		for i < n {
			c := s[i]
			d := int(c - '0')
			if d < 0 || d > 9 {
				break
			}
			flag = flag*10 + d
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
