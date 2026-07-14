package main

func run(rows []string) uint64 {
	const (
		prime = 1099511628211
		offset = 1469598103934665603
	)
	hash := offset
	for _, row := range rows {
		user, score, flag := 0, 0, false
		i, n := 0, len(row)
		for i < n {
			// find next quote
			for i < n && row[i] != '"' {
				i++
			}
			if i+6 >= n {
				break
			}
			// check field name
			switch row[i+1] {
			case 'u': // "user":
				if row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
					i += 7
					for i < n && (row[i] < '0' || row[i] > '9') {
						i++
					}
					for i < n && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
				}
			case 's': // "score":
				if row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
					i += 8
					for i < n && (row[i] < '0' || row[i] > '9') {
						i++
					}
					for i < n && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
				}
			case 'f': // "flag":true
				if row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' && row[i+7] == 't' {
					flag = true
					i += 8
				}
			}
			i++
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