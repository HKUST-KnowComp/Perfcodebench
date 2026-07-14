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
	// Find the start of the prefix
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

	// Find the end of the number (delimited by & or end of string)
	end := start
	for end < len(s) && s[end] != '&' {
		end++
	}

	// Manual integer parsing to avoid strconv overhead and allocations
	res := 0
	for i := start; i < end; i++ {
		c := s[i]
		if c >= '0' && c <= '9' {
			res = res*10 + int(c-'0')
		} else {
			break
		}
	}
	return res
}