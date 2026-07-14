package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u := parseValue(row, "u=")
		score := parseValue(row, "score=")
		flag := parseValue(row, "flag=")

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseValue(s, prefix string) int {
	// Find the start of the value after the prefix
	start := -1
	for i := 0; i <= len(s)-len(prefix); i++ {
		if s[i:i+len(prefix)] == prefix {
			start = i + len(prefix)
			break
		}
	}

	if start == -1 {
		return 0
	}

	// Parse integer until '&' or end of string
	res := 0
	for i := start; i < len(s); i++ {
		c := s[i]
		if c == '&' {
			break
		}
		if c < '0' || c > '9' {
			break
		}
		res = res*10 + int(c-'0')
	}
	return res
}