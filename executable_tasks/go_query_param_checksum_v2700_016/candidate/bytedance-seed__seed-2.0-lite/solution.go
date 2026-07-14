package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		b := []byte(row)
		// Parse u value
		var u int
		i := 2 // start after "u="
		for ; i < len(b) && b[i] >= '0' && b[i] <= '9'; i++ {
			u = u*10 + int(b[i]-'0')
		}
		i++ // skip trailing &

		// Parse score value
		var score int
		i += 6 // start after "score="
		for ; i < len(b) && b[i] >= '0' && b[i] <= '9'; i++ {
			score = score*10 + int(b[i]-'0')
		}
		i++ // skip trailing &

		// Parse flag value
		var flag int
		i += 5 // start after "flag="
		for ; i < len(b) && b[i] >= '0' && b[i] <= '9'; i++ {
			flag = flag*10 + int(b[i]-'0')
		}

		// Preserve exact hash calculation from baseline
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}