package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// Single pass scan to avoid strings.Split and multiple strings.Contains calls
		for i := 0; i < len(row); i++ {
			if row[i] == '"' {
				// Check for keys
				if i+7 <= len(row) && row[i+1:i+7] == "user":"" || (i+6 <= len(row) && row[i+1:i+7] == "user":) {
					// This is a simplified check for the specific key pattern
				}
				// More robust: look for the key start
				// Since we know the structure, we look for the specific key substrings
			}
			
			// Optimized approach: scan for key markers directly
		}
		
		// Re-implementing logic with a more efficient manual scan
		// We look for "user":, "score":, and "flag":true
		for i := 0; i < len(row); i++ {
			if row[i] == '"' {
				if i+7 <= len(row) && row[i+1:i+7] == "user":"" || (i+6 <= len(row) && row[i+1:i+7] == "user":) {
					// This is getting complex, let's use a simpler byte-based search
				}
			}
		}
		
		// Corrected high-performance scan
		idx := 0
		for idx < len(row) {
			if idx+7 <= len(row) && row[idx:idx+7] == "\"user\":" {
				idx += 7
				for idx < len(row) && (row[idx] < '0' || row[idx] > '9') {
					idx++
				}
				for idx < len(row) && row[idx] >= '0' && row[idx] <= '9' {
					user = user*10 + int(row[idx]-'0')
					idx++
				}
			} else if idx+8 <= len(row) && row[idx:idx+8] == "\"score\":" {
				idx += 8
				for idx < len(row) && (row[idx] < '0' || row[idx] > '9') {
					idx++
				}
				for idx < len(row) && row[idx] >= '0' && row[idx] <= '9' {
					score = score*10 + int(row[idx]-'0')
					idx++
				}
			} else if idx+12 <= len(row) && row[idx:idx+12] == "\"flag\":true" {
				flag = true
				idx += 12
			} else {
				idx++
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