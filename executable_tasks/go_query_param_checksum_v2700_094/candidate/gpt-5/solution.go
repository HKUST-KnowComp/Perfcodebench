package main

func run(rows []string) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	var hash uint64 = offset

	for _, row := range rows {
		s := row
		n := len(s)
		i := 0

		// Parse first integer (u)
		sign := 1
		for i < n {
			c := s[i]
			if c == '-' {
				sign = -1
				i++
				break
			}
			if c >= '0' && c <= '9' {
				break
			}
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
		u *= sign

		// Parse second integer (score)
		sign = 1
		for i < n {
			c := s[i]
			if c == '-' {
				sign = -1
				i++
				break
			}
			if c >= '0' && c <= '9' {
				break
			}
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
		score *= sign

		// Parse third integer (flag)
		sign = 1
		for i < n {
			c := s[i]
			if c == '-' {
				sign = -1
				i++
				break
			}
			if c >= '0' && c <= '9' {
				break
			}
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
		flag *= sign

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
