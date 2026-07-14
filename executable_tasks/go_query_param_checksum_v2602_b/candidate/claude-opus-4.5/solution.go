package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// row format: "u=<int>&score=<int>&flag=<int>"
		// We parse inline to avoid allocations from Split/TrimPrefix.

		// Find first '&' (end of u value)
		i := 0
		for row[i] != '&' {
			i++
		}
		// u value starts at index 2 (after "u=")
		u := parseIntFast(row, 2, i)

		// Find second '&' (end of score value)
		// score value starts at i+1+6 = i+7 (after "&score=")
		j := i + 7
		for row[j] != '&' {
			j++
		}
		score := parseIntFast(row, i+7, j)

		// flag value starts at j+1+5 = j+6 (after "&flag=")
		flag := parseIntFast(row, j+6, len(row))

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// parseIntFast parses a non-negative integer from s[start:end].
// Assumes valid decimal digits only (no sign, no whitespace).
func parseIntFast(s string, start, end int) int {
	n := 0
	for i := start; i < end; i++ {
		n = n*10 + int(s[i]-'0')
	}
	return n
}
