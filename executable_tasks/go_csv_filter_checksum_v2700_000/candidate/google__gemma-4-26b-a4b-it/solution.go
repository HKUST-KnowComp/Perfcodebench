package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// parts[2] is the 3rd field, parts[3] is the 4th, parts[4] is the 5th
		
		// Skip field 0 and 1
		idx := 0
		for i := 0; i < 2; i++ {
			for idx < len(row) && row[idx] != ',' {
				idx++
			}
			idx++ // skip comma
		}

		// Parse field 2 (b)
		bVal := 0
		for idx < len(row) && row[idx] != ',' {
			bVal = bVal*10 + int(row[idx]-'0')
			idx++
			}
		idx++ // skip comma

		// Parse field 3 (c)
		cVal := 0
		for idx < len(row) && row[idx] != ',' {
			cVal = cVal*10 + int(row[idx]-'0')
			idx++
			}
		idx++ // skip comma

		// Check field 4 (hot)
		// We need to check if the substring starting at idx is "hot"
		hot := false
		if idx+2 < len(row) && row[idx] == 'h' && row[idx+1] == 'o' && row[idx+2] == 't' {
			hot = true
		}

		v := uint64(bVal + cVal + 7)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}