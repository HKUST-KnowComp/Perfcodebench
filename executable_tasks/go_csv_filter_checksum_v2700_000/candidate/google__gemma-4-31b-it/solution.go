package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// We need fields at index 2, 3, and 4
		// Field 0
		pos := 0
		for pos < len(row) && row[pos] != ',' {
			pos++
		}
		pos++ // skip comma

		// Field 1
		for pos < len(row) && row[pos] != ',' {
			pos++
		}
		pos++ // skip comma

		// Field 2 (b)
		var b int
		for pos < len(row) && row[pos] != ',' {
			b = b*10 + int(row[pos]-'0')
			pos++
		}
		pos++ // skip comma

		// Field 3 (c)
		var c int
		for pos < len(row) && row[pos] != ',' {
			c = c*10 + int(row[pos]-'0')
			pos++
		}
		pos++ // skip comma

		// Field 4 (hot)
		// Check if the remaining part starts with "hot"
		hot := false
		if pos+2 < len(row) && row[pos] == 'h' && row[pos+1] == 'o' && row[pos+2] == 't' {
			hot = true
		}

		v := uint64(b + c + 7)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}