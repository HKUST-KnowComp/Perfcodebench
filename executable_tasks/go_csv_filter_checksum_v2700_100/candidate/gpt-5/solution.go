package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		var b, c int64
		hot := false

		// Scan fields separated by commas. We only need fields 2,3,4 (0-based).
		field := 0
		start := 0
		n := len(row)
		for i := 0; i <= n; i++ {
			if i == n || row[i] == ',' {
				if field == 2 {
					b = fastAtoi64Range(row, start, i)
				} else if field == 3 {
					c = fastAtoi64Range(row, start, i)
				} else if field == 4 {
					// Check for exact "hot"
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
					// No need to parse further fields
					break
				}
				field++
				start = i + 1
			}
		}

		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}

// fastAtoi64Range parses an integer in row[start:end]. Returns 0 on invalid input.
func fastAtoi64Range(row string, start, end int) int64 {
	if start >= end {
		return 0
	}
	i := start
	neg := false
	c := row[i]
	if c == '-' || c == '+' {
		neg = (c == '-')
		i++
		if i >= end {
			return 0
		}
	}
	var n int64
	for ; i < end; i++ {
		ch := row[i]
		if ch < '0' || ch > '9' {
			return 0
		}
		n = n*10 + int64(ch-'0')
	}
	if neg {
		n = -n
	}
	return n
}
