package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				part := row[start:i]
				
				// Check for keys in the current segment
				foundUser := false
				foundScore := false
				
				// Manual check for substrings to avoid strings.Contains overhead
				for j := 0; j < len(part); j++ {
					if !foundUser && j+6 < len(part) && part[j:j+7] == "\"user\":" {
						foundUser = true
					} else if !foundScore && j+7 < len(part) && part[j:j+8] == "\"score\":" {
						foundScore = true
					} else if !flag && j+10 < len(part) && part[j:j+11] == "\"flag\":true" {
						flag = true
					}
				}

				if foundUser {
					for j := 0; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
						}
					}
				} else if foundScore {
					for j := 0; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
						}
					}
				}
				
				start = i + 1
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}