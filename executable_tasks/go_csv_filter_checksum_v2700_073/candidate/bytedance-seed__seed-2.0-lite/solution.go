package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	const fnvPrime = uint64(1099511628211)
	for _, row := range rows {
		var commas [4]int
		count := 0
		// Locate first 4 commas to get offsets of required fields
		for i := 0; i < len(row) && count < 4; i++ {
			if row[i] == ',' {
				commas[count] = i
				count++
			}
		}
		// Parse b (original parts[2])
		var b int
		start := commas[1] + 1
		end := commas[2]
		for i := start; i < end; i++ {
			b = b*10 + int(row[i]-'0')
		}
		// Parse c (original parts[3])
		var c int
		start = commas[2] + 1
		end = commas[3]
		for i := start; i < end; i++ {
			c = c*10 + int(row[i]-'0')
		}
		// Check if 5th field is exactly "hot" (original parts[4] == "hot")
		hot := false
		if len(row) >= commas[3]+4 {
			if row[commas[3]+1] == 'h' && row[commas[3]+2] == 'o' && row[commas[3]+3] == 't' {
				if len(row) == commas[3]+4 || row[commas[3]+4] == ',' {
					hot = true
				}
			}
		}
		// Exact same hash update logic as baseline to preserve correctness
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}