package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c, commaCount int
		var val int
		var hot bool
		
		// Manual parsing to avoid Split and Atoi allocations
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				val = 0
				continue
			}
			if commaCount == 2 {
				b = b*10 + int(row[i]-'0')
			} else if commaCount == 3 {
				c = c*10 + int(row[i]-'0')
			} else if commaCount == 4 {
				if row[i] == 'h' {
					hot = true
				}
			}
		}

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= (v + 1)
		hash *= 1099511628211
	}
	return hash
}