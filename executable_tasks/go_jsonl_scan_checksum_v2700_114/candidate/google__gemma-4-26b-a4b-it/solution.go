package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		i := 0
		for i < len(row) {
			// Find the start of a potential key
			start := i
			for i < len(row) && row[i] != '"' {
				i++
			}
			if i >= len(row) {
				break
			}
			
			// Check for specific keys
			// "user":
			if i+7 <= len(row) && row[i:i+7] == "\"user\":" {
				i += 7
				for i < len(row) && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			} else if i+8 <= len(row) && row[i:i+8] == "\"score\":" {
				i += 8
				for i < len(row) && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			} else if i+11 <= len(row) && row[i:i+11] == "\"flag\":true" {
				flag = true
				i += 11
			} else {
				// Skip to next comma or end of field
				i++
			}

			// Advance to next comma to simulate Split behavior
			for i < len(row) && row[i] != ',' {
				i++
			}
			if i < len(row) {
				i++ // skip comma
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