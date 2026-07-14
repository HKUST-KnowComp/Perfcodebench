package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u := parseParam(row, "u=")
		score := parseParam(row, "score=")
		flag := parseParam(row, "flag=")

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseParam(s, prefix string) int {
	// Find the start of the value after the prefix
	// Since the format is fixed as u=..., score=..., flag=...
	// we can optimize the search.
	start := -1
	for i := 0; i < len(s); i++ {
		if i+len(prefix) <= len(s) && s[i:i+len(prefix)] == prefix {
			start = i + len(prefix)
			break
		}
	}

	if start == -1 {
		return 0
	}

	res := 0
	for i := start; i < len(s); i++ {
		c := s[i]
		if c < '0' || c > '9' {
			break
		}
		res = res*10 + int(c-'0')
	}
	return res
}