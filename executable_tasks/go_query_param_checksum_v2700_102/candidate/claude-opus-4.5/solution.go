package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Format: u=<int>&score=<int>&flag=<int>
		// Parse u value (starts at index 2, after "u=")
		i := 2
		u := 0
		for row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		// Skip "&score=" (7 chars)
		i += 7
		score := 0
		for row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		// Skip "&flag=" (6 chars)
		i += 6
		flag := 0
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