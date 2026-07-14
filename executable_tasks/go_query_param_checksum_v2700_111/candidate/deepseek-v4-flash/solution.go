package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		i := 0
		// Parse u=...
		i = skipToEqual(row, i) // skip "u"
		i, u = parseInt(row, i)
		// Parse &score=...
		i = skipToEqual(row, i+1) // skip '&' before score
		i, score = parseInt(row, i)
		// Parse &flag=...
		i = skipToEqual(row, i+1) // skip '&' before flag
		i, flag = parseInt(row, i)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// skipToEqual advances index past the field name and the '=', returns index of first digit.
func skipToEqual(s string, start int) int {
	for start < len(s) && s[start] != '=' {
		start++
	}
	return start + 1 // skip '='
}

// parseInt reads digits starting at start, returns next index after last digit and integer value.
func parseInt(s string, start int) (int, int) {
	val := 0
	for start < len(s) && s[start] >= '0' && s[start] <= '9' {
		val = val*10 + int(s[start]-'0')
		start++
	}
	return start, val
}
