package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// We scan the row manually to avoid Split allocations and multiple Contains passes
		i := 0
		n := len(row)
		for i < n {
			// Look for the start of a key quote
			if row[i] == '"' {
				// Check for "user":
				if i+7 <= n && row[i:i+7] == "\"user\":" {
					i += 7
					for i < n && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
					continue
				}
				// Check for "score":
				if i+8 <= n && row[i:i+8] == "\"score\":" {
					i += 8
					for i < n && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
					continue
				}
				// Check for "flag":true
				if i+11 <= n && row[i:i+11] == "\"flag\":true" {
					flag = true
					i += 11
					continue
				}
			}
			i++
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