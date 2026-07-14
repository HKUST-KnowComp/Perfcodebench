package main

func run(rows []string) uint64 {
	var hash uint64 = 146959810394665603
	// Note: The original hash constant was 1469598103934665603.
	// Re-aligning to the exact provided baseline constant to ensure correctness.
	hash = 1469598103934665603

	for _, row := range rows {
		// We need parts[2], parts[3], and parts[4]
		// parts[0], parts[1], parts[2], parts[3], parts[4]
		
		idx := 0
		commaCount := 0
		
		var b, c uint64
		var hot bool
		
		// Skip parts[0] and parts[1]
		for i := 0; i < 2; i++ {
			for idx < len(row) && row[idx] != ',' {
				idx++
			}
			idx++ // skip comma
		}

		// Parse parts[2] (b)
		start := idx
		for idx < len(row) && row[idx] != ',' {
			idx++
		}
		for i := start; i < idx; i++ {
			b = b*10 + uint64(row[i]-'0')
		}
		idx++ // skip comma

		// Parse parts[3] (c)
		start = idx
		for idx < len(row) && row[idx] != ',' {
			idx++
		}
		for i := start; i < idx; i++ {
			c = c*10 + uint64(row[i]-'0')
		}
		idx++ // skip comma

		// Parse parts[4] (hot)
		start = idx
		for idx < len(row) && row[idx] != ',' {
			idx++
		}
		// Check if parts[4] is "hot"
		// We check length and content to avoid allocation
		if idx-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
			hot = true
		}

		v := b + c + 11
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}