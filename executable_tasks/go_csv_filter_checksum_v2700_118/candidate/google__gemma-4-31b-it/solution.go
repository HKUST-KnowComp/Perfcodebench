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
			if row[pos] >= '0' && row[pos] <= '9' {
				b = b*10 + int(row[pos]-'0')
			}
			pos++
		}
		pos++ // skip comma

		// Field 3 (c)
		var c int
		for pos < len(row) && row[pos] != ',' {
			if row[pos] >= '0' && row[pos] <= '9' {
				c = c*10 + int(row[pos]-'0')
			}
			pos++
		}
		pos++ // skip comma

		// Field 4 (hot)
		// Check if the remaining part starts with "hot"
		isHot := false
		if len(row)-pos >= 3 && row[pos] == 'h' && row[pos+1] == 'o' && row[pos+2] == 't' {
			isHot = true
		}

		v := uint64(b + c + 10)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}