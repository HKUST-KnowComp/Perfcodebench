package main

func fastAtoi(s string) int {
	var res int
	for i := 0; i < len(s); i++ {
		res = res*10 + int(s[i]-'0')
	}
	return res
}

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		var commas [4]int
		count := 0
		// Only scan until we find the 4 commas needed to extract required fields
		for i := 0; i < len(row) && count < 4; i++ {
			if row[i] == ',' {
				commas[count] = i
				count++
			}
		}
		// Parse required integer fields directly from row substrings (no full split)
		b := fastAtoi(row[commas[1]+1 : commas[2]])
		c := fastAtoi(row[commas[2]+1 : commas[3]])
		// Check "hot" flag without creating an additional substring
		hot := row[commas[3]+1] == 'h' && row[commas[3]+2] == 'o' && row[commas[3]+3] == 't'
		// Exact same hash calculation as baseline to preserve correctness
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}