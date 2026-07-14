package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var commaPos [4]int // Stores indices of 1st, 2nd, 3rd, 4th comma to avoid full split
		found := 0
		for i := 0; i < len(row) && found < 4; i++ {
			if row[i] == ',' {
				commaPos[found] = i
				found++
			}
		}
		// Parse b from parts[2], no allocations
		var b int
		for i := commaPos[1] + 1; i < commaPos[2]; i++ {
			b = b*10 + int(row[i]-'0')
		}
		// Parse c from parts[3], no allocations
		var c int
		for i := commaPos[2] + 1; i < commaPos[3]; i++ {
			c = c*10 + int(row[i]-'0')
		}
		// Check if parts[4] == "hot" with direct byte checks
		hot := len(row) >= commaPos[3]+4 && row[commaPos[3]+1] == 'h' && row[commaPos[3]+2] == 'o' && row[commaPos[3]+3] == 't'
		// Exact same calculation logic as original to preserve correctness
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}