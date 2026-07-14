package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		// Parse 'u' starting at index 2 (after "u=")
		i := 2
		u := 0
		uNeg := false
		if row[i] == '-' {
			uNeg = true
			i++
		}
		for row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		if uNeg {
			u = -u
		}

		// Parse 'score' starting after "&score=" (7 chars)
		i += 7
		score := 0
		scoreNeg := false
		if row[i] == '-' {
			scoreNeg = true
			i++
		}
		for row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		if scoreNeg {
			score = -score
		}

		// Parse 'flag' starting after "&flag=" (6 chars)
		i += 6
		flag := 0
		flagNeg := false
		if i < len(row) && row[i] == '-' {
			flagNeg = true
			i++
		}
		for i < len(row) {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		if flagNeg {
			flag = -flag
		}

		// Calculate checksum component
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}

	return hash
}