package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 2
		u := 0
		neg := false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + int(c-'0')
		}
		if neg {
			u = -u
		}

		i += 7
		score := 0
		neg = false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + int(c-'0')
		}
		if neg {
			score = -score
		}

		i += 6
		flag := 0
		neg = false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			flag = flag*10 + int(c-'0')
		}
		if neg {
			flag = -flag
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}