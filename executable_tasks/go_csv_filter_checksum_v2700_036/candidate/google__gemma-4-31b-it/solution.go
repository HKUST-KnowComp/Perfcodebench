package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		// Skip first two columns
		pos := 0
		for i := 0; i < 2; i++ {
			for pos < len(row) && row[pos] != ',' {
				pos++
			}
			pos++
		}

		// Parse column 2 (b)
		var b int
		for pos < len(row) && row[pos] != ',' {
			b = b*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Parse column 3 (c)
		var c int
		for pos < len(row) && row[pos] != ',' {
			c = c*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Check column 4 for "hot"
		hot := false
		if pos+2 < len(row) && row[pos] == 'h' && row[pos+1] == 'o' && row[pos+2] == 't' {
			hot = true
		}

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}