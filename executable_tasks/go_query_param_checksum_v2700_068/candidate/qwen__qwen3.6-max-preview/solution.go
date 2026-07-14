package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u := 0
		score := 0
		flag := 0

		i := 2 // skip "u="
		for i < len(row) {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + int(c-'0')
			i++
		}

		i += 7 // skip "&score="
		for i < len(row) {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + int(c-'0')
			i++
		}

		i += 6 // skip "&flag="
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
