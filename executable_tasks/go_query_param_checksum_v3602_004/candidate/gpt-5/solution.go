package main

func run(rows []string) uint64 {
	const (
		hashOffset = 1469598103934665603
		hashPrime  = 1099511628211
	)

	hash := uint64(hashOffset)
	for _, s := range rows {
		n := len(s)
		i := 0

		// Parse first value (u)
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++ // skip '='
		}
		u := 0
		sign := 1
		if i < n && s[i] == '-' {
			sign = -1
			i++
		}
		for i < n {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + int(c-'0')
			i++
		}
		u *= sign

		// Parse second value (score)
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++ // skip '='
		}
		score := 0
		sign = 1
		if i < n && s[i] == '-' {
			sign = -1
			i++
		}
		for i < n {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + int(c-'0')
			i++
		}
		score *= sign

		// Parse third value (flag)
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++ // skip '='
		}
		flag := 0
		sign = 1
		if i < n && s[i] == '-' {
			sign = -1
			i++
		}
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
		hash *= hashPrime
	}
	return hash
}
