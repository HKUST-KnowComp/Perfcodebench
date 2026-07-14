package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				part := row[start:i]
				if len(part) >= 7 && part[0] == '"' {
					// Check for "user":
					if len(part) >= 7 && part[1] == 'u' && part[2] == 's' && part[3] == 'e' && part[4] == 'r' && part[5] == '"' && part[6] == ':' {
						for j := 7; j < len(part); j++ {
							if part[j] >= '0' && part[j] <= '9' {
								user = user*10 + int(part[j]-'0')
							}
						}
					}
					// Check for "score":
					if len(part) >= 8 && part[1] == 's' && part[2] == 'c' && part[3] == 'o' && part[4] == 'r' && part[5] == 'e' && part[6] == '"' && part[7] == ':' {
						for j := 8; j < len(part); j++ {
							if part[j] >= '0' && part[j] <= '9' {
								score = score*10 + int(part[j]-'0')
							}
						}
					}
					// Check for "flag":true
					if len(part) >= 12 && part[1] == 'f' && part[2] == 'l' && part[3] == 'a' && part[4] == 'g' && part[5] == '"' && part[6] == ':' && part[7] == 't' && part[8] == 'r' && part[9] == 'u' && part[10] == 'e' {
						flag = true
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
		hash *= 1099511628211
	}
	return hash
}