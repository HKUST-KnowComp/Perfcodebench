package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	const fnvPrime = 1099511628211
	for _, row := range rows {
		var user, score int
		var flag bool
		n := len(row)
		i := 0
		for i < n {
			// Match "user":
			if i+6 <= n && row[i] == '"' && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == ':' {
				i += 6
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			} else if i+7 <= n && row[i] == '"' && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == ':' {
				// Match "score":
				i += 7
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			} else if i+10 <= n && row[i] == '"' && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == ':' && row[i+6] == 't' && row[i+7] == 'r' && row[i+8] == 'u' && row[i+9] == 'e' {
				// Match "flag":true
				flag = true
				i += 10
			} else {
				i++
			}
		}
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}