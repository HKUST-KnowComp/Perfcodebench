package main

func run(rows []string) uint64 {
	const offset64 = 1469598103934665603
	const prime64 = 1099511628211

	var hash uint64 = offset64
	for _, s := range rows {
		n := len(s)
		i := 0

		// Find first '=' and parse u
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++
		}
		u, i := parseIntFrom(s, i, n)

		// Find second '=' and parse score
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++
		}
		score, i := parseIntFrom(s, i, n)

		// Find third '=' and parse flag
		for i < n && s[i] != '=' {
			i++
		}
		if i < n {
			i++
		}
		flag, _ := parseIntFrom(s, i, n)

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}

func parseIntFrom(s string, i, n int) (int, int) {
	if i >= n {
		return 0, i
	}
	sign := 1
	if s[i] == '-' {
		sign = -1
		i++
	}
	val := 0
	for i < n {
		c := s[i]
		if c < '0' || c > '9' {
			break
		}
		val = val*10 + int(c-'0')
		i++
	}
	return val * sign, i
}
