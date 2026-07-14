package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var bVal, cVal int
		var hot bool

		start := 0
		field := 0
		n := len(row)
		for i := 0; i <= n; i++ {
			var isDelim bool
			if i == n || row[i] == ',' {
				isDelim = true
			}
			if isDelim {
				if field == 2 {
					bVal = parseIntStrict(row, start, i)
				} else if field == 3 {
					cVal = parseIntStrict(row, start, i)
				} else if field == 4 {
					if i-start == 3 {
						if row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
							hot = true
						}
					}
				}
				field++
				if field > 4 || i == n {
					break
				}
				start = i + 1
			}
		}

		v := uint64(bVal + cVal + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// parseIntStrict parses an integer from s[start:end]. If the substring is not a valid
// base-10 integer (optional leading sign, then digits only), it returns 0.
func parseIntStrict(s string, start, end int) int {
	if start >= end {
		return 0
	}
	i := start
	neg := false
	switch s[i] {
	case '-':
		neg = true
		i++
		if i == end {
			return 0
		}
	case '+':
		i++
		if i == end {
			return 0
		}
	}
	var v int
	for ; i < end; i++ {
		c := s[i]
		if c < '0' || c > '9' {
			return 0
		}
		v = v*10 + int(c-'0')
	}
	if neg {
		v = -v
	}
	return v
}
