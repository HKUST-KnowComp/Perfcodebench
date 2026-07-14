package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		pos := 0
		// Parse u, skip "u=" prefix of length 2
		u := 0
		pos += 2
		for pos < len(row) && row[pos] != '&' {
			u = u*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Parse score, skip "score=" prefix of length 6
		score := 0
		pos += 6
		for pos < len(row) && row[pos] != '&' {
			score = score*10 + int(row[pos]-'0')
			pos++
		}
		pos++

		// Parse flag, skip "flag=" prefix of length 5
		flag := 0
		pos += 5
		for pos < len(row) && row[pos] != '&' {
			flag = flag*10 + int(row[pos]-'0')
			pos++
		}

		// Exact same calculation as original to preserve checksum
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}