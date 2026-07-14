package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var commas [4]int
		count := 0
		for i := 0; i < len(row) && count < 4; i++ {
			if row[i] == ',' {
				commas[count] = i
				count++
			}
		}
		// Parse b from third field (original parts[2])
		b := 0
		for i := commas[1] + 1; i < commas[2]; i++ {
			b = b*10 + int(row[i]-'0')
		}
		// Parse c from fourth field (original parts[3])
		c := 0
		for i := commas[2] + 1; i < commas[3]; i++ {
			c = c*10 + int(row[i]-'0')
		}
		// Check if fifth field is exactly "hot" (original parts[4] == "hot")
		hot := false
		hotStart := commas[3] + 1
		if len(row) >= hotStart+3 {
			if row[hotStart] == 'h' && row[hotStart+1] == 'o' && row[hotStart+2] == 't' {
				if len(row) == hotStart+3 || row[hotStart+3] == ',' {
					hot = true
				}
			}
		}
		// Exact same hash calculation as baseline to preserve checksum correctness
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}