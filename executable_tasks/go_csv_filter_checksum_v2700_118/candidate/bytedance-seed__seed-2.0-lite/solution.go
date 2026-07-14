package main

func run(rows []string) uint64 {
	const offsetBasis uint64 = 14695981039346656037
	const fnvPrime uint64 = 1099511628211
	hash := offsetBasis

	for _, row := range rows {
		var commas [5]int
		count := 0
		for i := 0; i < len(row) && count < 5; i++ {
			if row[i] == ',' {
				commas[count] = i
				count++
			}
		}

		// Parse b from parts[2] (between 1st and 2nd comma after first two fields)
		var b int
		for _, ch := range row[commas[1]+1 : commas[2]] {
			b = b*10 + int(ch-'0')
		}

		// Parse c from parts[3]
		var c int
		for _, ch := range row[commas[2]+1 : commas[3]] {
			c = c*10 + int(ch-'0')
		}

		// Check if parts[4] is exactly "hot"
		hot := len(row) >= commas[3]+4 &&
			row[commas[3]+1] == 'h' &&
			row[commas[3]+2] == 'o' &&
			row[commas[3]+3] == 't' &&
			(len(row) == commas[3]+4 || row[commas[3]+4] == ',')

		// Exact same hash calculation as baseline
		v := uint64(b + c + 10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}