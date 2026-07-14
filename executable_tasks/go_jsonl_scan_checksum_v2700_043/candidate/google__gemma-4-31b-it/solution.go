package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Scan the row manually to avoid strings.Split and strings.Contains allocations
		for i := 0; i < len(row); {
			if row[i] == '"' {
				// Check for "user":
				if i+6 < len(row) && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' {
					i += 6
					// Skip to the value
					for i < len(row) && row[i] != '0' && row[i] != '1' && row[i] != '2' && row[i] != '3' && row[i] != '4' && row[i] != '5' && row[i] != '6' && row[i] != '7' && row[i] != '8' && row[i] != '9' {
						i++
					}
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
					continue
				}
				// Check for "score":
				if i+7 < len(row) && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' {
					i += 7
					for i < len(row) && row[i] != '0' && row[i] != '1' && row[i] != '2' && row[i] != '3' && row[i] != '4' && row[i] != '5' && row[i] != '6' && row[i] != '7' && row[i] != '8' && row[i] != '9' {
						i++
					}
					for i < len(row) && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
					continue
				}
				// Check for "flag":
				if i+6 < len(row) && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' {
					i += 6
					for i < len(row) && row[i] != 't' {
						i++
					}
					if i+4 < len(row) && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
						flag = true
						i += 4
					}
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