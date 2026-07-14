package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		s := row
		n := len(s)
		i := 2 // skip "u="

		// parse u
		sign := 1
		if i < n {
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
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
		u *= sign

		// skip '&'
		if i < n {
			if s[i] == '&' {
				i++
			} else {
				for i < n && s[i] != '&' {
					i++
				}
				if i < n {
					i++
				}
			}
		}

		// skip "score="
		if i+6 <= n {
			i += 6
		} else {
			i = n
		}

		// parse score
		sign = 1
		if i < n {
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
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
		score *= sign

		// skip '&'
		if i < n {
			if s[i] == '&' {
				i++
			} else {
				for i < n && s[i] != '&' {
					i++
				}
				if i < n {
					i++
				}
			}
		}

		// skip "flag="
		if i+5 <= n {
			i += 5
		} else {
			i = n
		}

		// parse flag
		sign = 1
		if i < n {
			if s[i] == '-' {
				sign = -1
				i++
			} else if s[i] == '+' {
				i++
			}
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
		flag *= sign

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
