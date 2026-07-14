package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int64
		hot := false

		field := 0
		start := 0
		n := len(row)

	rowScan:
		for i := 0; i <= n; i++ {
			if i == n || row[i] == ',' {
				// Process field [start, i)
				switch field {
				case 2: // third field -> b
					b = atoiField64(row, start, i)
				case 3: // fourth field -> c
					c = atoiField64(row, start, i)
				case 4: // fifth field -> check hot
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
					// We do not need fields beyond the 5th
					break rowScan
				}
				field++
				start = i + 1
			}
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// atoiField64 parses a signed integer from s[start:end].
// Returns 0 on empty or invalid content, matching strconv.Atoi behavior when error is ignored.
func atoiField64(s string, start, end int) int64 {
	if start >= end {
		return 0
	}
	i := start
	neg := false
	c := s[i]
	if c == '-' || c == '+' {
		neg = c == '-'
		i++
		if i >= end {
			return 0
		}
	}
	var val int64
	for ; i < end; i++ {
		ch := s[i]
		if ch < '0' || ch > '9' {
			return 0
		}
		val = val*10 + int64(ch-'0')
	}
	if neg {
		val = -val
	}
	return val
}
