package main

func run(rows []string) uint64 {
	const (
		startHash = 1469598103934665603
		multiplier = 1099511628211
	)
	hash := uint64(startHash)

	for _, row := range rows {
		var b, c int
		var hot bool

		// single‑pass scanner to extract fields 2,3,4 (0‑based)
		fieldIdx := 0
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				switch fieldIdx {
				case 2:
					b = atoi(row[start:i])
				case 3:
					c = atoi(row[start:i])
				case 4:
					hot = row[start:i] == "hot"
				}
				fieldIdx++
				start = i + 1
				if fieldIdx > 4 {
					break
				}
			}
		}

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= multiplier
	}
	return hash
}

// fast integer parser, equivalent to strconv.Atoi but without error handling.
// Handles optional leading '-' sign. Assumes valid input.
func atoi(s string) int {
	if len(s) == 0 {
		return 0
	}
	neg := false
	i := 0
	if s[0] == '-' {
		neg = true
		i = 1
	}
	n := 0
	for ; i < len(s); i++ {
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}
