package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// We need parts[2], parts[3], and parts[4]
		// Skip parts[0] and parts[1]
		pos := 0
		for i := 0; i < 2; i++ {
			for pos < len(row) && row[pos] != ',' {
				pos++
			}
			pos++
		}

		// Parse parts[2] (b)
		var b int
		for pos < len(row) && row[pos] != ',' {
			b = b*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Parse parts[3] (c)
		var c int
		for pos < len(row) && row[pos] != ',' {
			c = c*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Check parts[4] == "hot"
		// We only need to check if the remaining string starts with "hot"
		hot := false
		if pos+2 < len(row) && row[pos] == 'h' && row[pos+1] == 'o' && row[pos+2] == 't' {
			hot = true
		}

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}