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

func parseValue(row, prefix string) int {
	// Find the start of the prefix
	startIdx := -1
	for i := 0; i <= len(row)-len(prefix); i++ {
		if row[i:i+len(prefix)] == prefix {
			startIdx = i + len(prefix)
			break
		}
	}

	if startIdx == -1 {
		return 0
	}

	// Find the end of the number (delimited by & or end of string)
	endIdx := startIdx
	for endIdx < len(row) && row[endIdx] != '&' {
		endIdx++
	}

	// Manual integer parsing to avoid strconv overhead and allocations
	val := 0
	for i := startIdx; i < endIdx; i++ {
		c := row[i]
		if c >= '0' && c <= '9' {
			val = val*10 + int(c-'0')
		} else {
			break
		}
	}
	return val
}