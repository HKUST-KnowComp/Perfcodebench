package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// We need parts[2], parts[3], and parts[4]
		start := 0
		commaCount := 0
		var b, c int
		var hot bool

		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					start = i + 1
				} else if commaCount == 3 {
					// Parse b
					for j := start; j < i; j++ {
						b = b*10 + int(row[j]-'0')
					}
					start = i + 1
				} else if commaCount == 4 {
					// Parse c
					for j := start; j < i; j++ {
						c = c*10 + int(row[j]-'0')
					}
					start = i + 1
					break
				}
		}

		// Check if parts[4] == "hot"
		// The remaining slice from 'start' to end is parts[4]
		remaining := row[start:]
		if len(remaining) == 3 && remaining[0] == 'h' && remaining[1] == 'o' && remaining[2] == 't' {
			hot = true
		}

		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}