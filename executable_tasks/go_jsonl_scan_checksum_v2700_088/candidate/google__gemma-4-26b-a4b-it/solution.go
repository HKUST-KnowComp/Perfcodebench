package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// We scan the row manually to avoid the overhead of strings.Split and multiple Contains calls.
		// Since the input is JSON-like and comma-separated, we look for the key patterns directly.
		for i := 0; i < len(row); i++ {
			if i+7 <= len(row) && row[i:i+7] == "\"user\":" {
				i += 7
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
				i-- // Adjust for loop increment
			} else if i+8 <= len(row) && row[i:i+8] == "\"score\":" {
				i += 8
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
				i-- // Adjust for loop increment
			} else if i+11 <= len(row) && row[i:i+11] == "\"flag\":true" {
				flag = true
				i += 10
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