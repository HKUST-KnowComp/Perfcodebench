package main

const comma = ','

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		n := len(row)
		for i := 0; i < n; i++ {
			if row[i] == '"' && i+6 < n && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
				// "user":
				i += 7
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			}
			if row[i] == '"' && i+7 < n && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
				// "score":
				i += 8
				for i < n && (row[i] < '0' || row[i] > '9') {
					i++
				}
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			}
			if row[i] == '"' && i+6 < n && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' {
				// "flag":
				i += 7
				for i < n && (row[i] == ' ' || row[i] == '\t') {
					i++
				}
				if i < n && row[i] == 't' && i+4 < n && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
					flag = true
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