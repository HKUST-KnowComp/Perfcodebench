package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag uint64
		
		// Manual single-pass scan to avoid Split and TrimPrefix allocations
		// Expected format: u=VAL&score=VAL&flag=VAL
		
		// Parse u
		idx := 2 // skip "u="
		for idx < len(row) && row[idx] != '&' {
			u = u*10 + uint64(row[idx]-'0')
			idx++
		}
		
		// Skip '&score='
		idx += 7
		for idx < len(row) && row[idx] != '&' {
			score = score*10 + uint64(row[idx]-'0')
			idx++
		}
		
		// Skip '&flag='
		idx += 6
		for idx < len(row) {
			flag = flag*10 + uint64(row[idx]-'0')
			idx++
		}

		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}