package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// Single pass through the row
		i := 0
		n := len(row)
		foundFields := 0
		
		for i < n && foundFields < 3 {
			// Look for field markers
			if i+7 <= n && row[i:i+7] == "\"user\":" {
				i += 7
				// Skip whitespace
				for i < n && (row[i] == ' ' || row[i] == '\t') {
					i++
				}
				// Parse number
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
				foundFields++
			} else if i+8 <= n && row[i:i+8] == "\"score\":" {
				i += 8
				// Skip whitespace
				for i < n && (row[i] == ' ' || row[i] == '\t') {
					i++
				}
				// Parse number
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
				foundFields++
			} else if i+12 <= n && row[i:i+12] == "\"flag\":true" {
				flag = true
				i += 12
				foundFields++
			} else {
				i++
			}
		}
		
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
