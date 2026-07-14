package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		i := 2 // skip "u="
		u := parseUint(row, &i)
		i += 7 // skip "&score="
		score := parseUint(row, &i)
		i += 6 // skip "&flag="
		flag := parseUint(row, &i)

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// parseUint reads a non-negative integer from s starting at index i.
// It advances i past the digits and returns the integer. Returns 0 if no digits found.
func parseUint(s string, i *int) uint64 {
	var n uint64
	for *i < len(s) && s[*i] >= '0' && s[*i] <= '9' {
		n = n*10 + uint64(s[*i]-'0')
		*i++
	}
	return n
}
