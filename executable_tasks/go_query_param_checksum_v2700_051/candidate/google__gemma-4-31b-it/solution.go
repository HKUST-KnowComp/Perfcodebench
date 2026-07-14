package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		
		// Manual scan for u=, score=, flag=
		// We assume the format is consistent as per the baseline
		i := 0
		
		// Parse u
		i += 2 // skip "u="
		for i < len(row) && row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		
		// Parse score
		i += 1 // skip '&'
		i += 6 // skip "score="
		for i < len(row) && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		
		// Parse flag
		i += 1 // skip '&'
		i += 5 // skip "flag="
		for i < len(row) {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}