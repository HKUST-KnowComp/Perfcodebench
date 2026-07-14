package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		// We need parts[2], parts[3], and parts[4]
		// Skip first two commas
		start := 0
		commaCount := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					start = i + 1
					break
				}
			}
		}

		// Parse parts[2] (b)
		var b int
		idx := start
		for idx < len(row) && row[idx] != ',' {
			b = b*10 + int(row[idx]-'0')
			idx++
		}

		// Skip comma to parts[3]
		idx++
		var c int
		for idx < len(row) && row[idx] != ',' {
			c = c*10 + int(row[idx]-'0')
			idx++
		}

		// Check parts[4] == "hot"
		idx++ // skip comma
		var hot bool
		if idx+2 < len(row) && row[idx] == 'h' && row[idx+1] == 'o' && row[idx+2] == 't' {
			hot = true
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}