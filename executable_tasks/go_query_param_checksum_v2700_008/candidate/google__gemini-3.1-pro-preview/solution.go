package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		var u, score, flag int
		var i int

		// Parse 'u' - starts at index 2 (after "u=")
		i = 2
		neg := false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for i < len(row) && row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		if neg {
			u = -u
		}

		// Parse 'score' - starts after "&score=" (7 chars)
		i += 7
		neg = false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for i < len(row) && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		if neg {
			score = -score
		}

		// Parse 'flag' - starts after "&flag=" (6 chars)
		i += 6
		neg = false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for i < len(row) {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		if neg {
			flag = -flag
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}
	return hash
}