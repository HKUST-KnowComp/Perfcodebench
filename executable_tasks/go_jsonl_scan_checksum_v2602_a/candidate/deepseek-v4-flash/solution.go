package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		for i := 0; i < len(row); i++ {
			// Look for "\"user\":"
			if i+7 < len(row) && row[i] == '"' && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
				j := i + 7
				for j < len(row) && row[j] >= '0' && row[j] <= '9' {
					user = user*10 + int(row[j]-'0')
					j++
				}
				i = j - 1
			} else if i+8 < len(row) && row[i] == '"' && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
				j := i + 8
				for j < len(row) && row[j] >= '0' && row[j] <= '9' {
					score = score*10 + int(row[j]-'0')
					j++
				}
				i = j - 1
			} else if i+12 < len(row) && row[i] == '"' && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' && row[i+7] == 't' && row[i+8] == 'r' && row[i+9] == 'u' && row[i+10] == 'e' {
				flag = true
				i += 11
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
