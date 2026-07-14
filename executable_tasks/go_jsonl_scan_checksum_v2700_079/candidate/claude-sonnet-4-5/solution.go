package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		foundFields := 0
		
		for i := 0; i < len(row) && foundFields < 3; i++ {
			if row[i] == '"' && i+1 < len(row) {
				switch row[i+1] {
				case 'u':
					if i+6 < len(row) && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
						i += 7
						for i < len(row) && row[i] >= '0' && row[i] <= '9' {
							user = user*10 + int(row[i]-'0')
							i++
						}
						foundFields++
					}
				case 's':
					if i+7 < len(row) && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
						i += 8
						for i < len(row) && row[i] >= '0' && row[i] <= '9' {
							score = score*10 + int(row[i]-'0')
							i++
						}
						foundFields++
					}
				case 'f':
					if i+6 < len(row) && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' {
						i += 7
						if i+3 < len(row) && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
							flag = true
						}
						foundFields++
					}
				}
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
