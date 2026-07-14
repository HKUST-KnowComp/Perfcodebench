package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u := 0
		score := 0
		flag := 0

		// Parse u, skip "u=" (2 chars)
		i := 2
		for i < len(row) {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + int(c-'0')
			i++
		}

		// Parse score, skip "&score=" (7 chars)
		i += 7
		for i < len(row) {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + int(c-'0')
			i++
		}

		// Parse flag, skip "&flag=" (6 chars)
		i += 6
		for i < len(row) {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			flag = flag*10 + int(c-'0')
			i++
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
