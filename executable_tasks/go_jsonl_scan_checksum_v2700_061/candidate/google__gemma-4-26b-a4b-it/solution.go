package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// We scan the row manually to avoid Split allocations and multiple Contains passes
		r := []byte(row)
		n := len(r)
		i := 0
		for i < n {
			// Look for the start of a key (quote)
			if r[i] == '"' {
				// Check for "user":
				if i+7 < n && r[i+1] == 'u' && r[i+2] == 's' && r[i+3] == 'e' && r[i+4] == 'r' && r[i+5] == '"' && r[i+6] == ':' {
					i += 7
					// Parse digits
					for i < n && r[i] >= '0' && r[i] <= '9' {
						user = user*10 + int(r[i]-'0')
						i++
					}
					continue
				}
				// Check for "score":
				if i+8 < n && r[i+1] == 's' && r[i+2] == 'c' && r[i+3] == 'o' && r[i+4] == 'r' && r[i+5] == 'e' && r[i+6] == '"' && r[i+7] == ':' {
					i += 8
					// Parse digits
					for i < n && r[i] >= '0' && r[i] <= '9' {
						score = score*10 + int(r[i]-'0')
						i++
					}
					continue
				}
				// Check for "flag":true
				if i+11 < n && r[i+1] == 'f' && r[i+2] == 'l' && r[i+3] == 'a' && r[i+4] == 'g' && r[i+5] == '"' && r[i+6] == ':' && r[i+7] == 't' && r[i+8] == 'r' && r[i+9] == 'u' && r[i+10] == 'e' {
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