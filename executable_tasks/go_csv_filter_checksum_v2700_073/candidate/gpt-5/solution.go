package main

func run(rows []string) uint64 {
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	hash := fnvOffset
	for _, s := range rows {
		bVal := 0
		cVal := 0
		hot := false

		start := 0
		field := 0
		n := len(s)
		for i := 0; i <= n; i++ {
			var ch byte
			if i < n {
				ch = s[i]
			} else {
				// Treat end of string as a delimiter to process the last field
				ch = ','
			}
			if ch == ',' {
				end := i
				if field == 2 { // third column
					bVal = atoiRange(s, start, end)
				} else if field == 3 { // fourth column
					cVal = atoiRange(s, start, end)
				} else if field == 4 { // fifth column
					// exact match to "hot"
					if end-start == 3 && s[start] == 'h' && s[start+1] == 'o' && s[start+2] == 't' {
						hot = true
					}
					// We have all needed fields; can stop scanning
					// Break out of the loop early to avoid scanning remaining columns
					break
				}
				field++
				start = i + 1
			}
		}

		v := uint64(bVal + cVal + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}

// atoiRange parses a decimal integer in s[start:end].
// It supports an optional leading '+' or '-' and stops at the first non-digit.
// If no digits are found, it returns 0.
func atoiRange(s string, start, end int) int {
	if start >= end {
		return 0
	}
	i := start
	neg := false
	c := s[i]
	if c == '-' {
		neg = true
		i++
	} else if c == '+' {
		i++
	}
	if i >= end {
		return 0
	}
	n := 0
	parsed := false
	for i < end {
		ch := s[i]
		if ch < '0' || ch > '9' {
			break
		}
		parsed = true
		n = n*10 + int(ch-'0')
		i++
	}
	if !parsed {
		return 0
	}
	if neg {
		return -n
	}
	return n
}
