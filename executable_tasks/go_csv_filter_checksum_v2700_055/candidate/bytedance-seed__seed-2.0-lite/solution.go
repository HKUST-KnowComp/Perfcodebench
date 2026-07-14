package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)

	for _, row := range rows {
		var commas [5]int
		commaCount := 0
		n := len(row)
		for i := 0; i < n && commaCount < 5; i++ {
			if row[i] == ',' {
				commas[commaCount] = i
				commaCount++
			}
		}

		// Parse b from parts[2]
		bStr := row[commas[2]+1 : commas[3]]
		var b int
		for _, c := range bStr {
			b = b*10 + int(c-'0')
		}

		// Parse c from parts[3]
		cStr := row[commas[3]+1 : commas[4]]
		var c int
		for _, ch := range cStr {
			c = c*10 + int(ch-'0')
		}

		// Check if parts[4] == "hot"
		hot := len(row) >= commas[4]+4 && row[commas[4]+1] == 'h' && row[commas[4]+2] == 'o' && row[commas[4]+3] == 't'

		// Exact original calculation logic preserved
		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}