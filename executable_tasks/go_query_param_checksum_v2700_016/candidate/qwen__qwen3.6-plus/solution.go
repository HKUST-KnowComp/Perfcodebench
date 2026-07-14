package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag uint64
		i := 2

		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + uint64(c-'0')
		}
		i += 7

		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + uint64(c-'0')
		}
		i += 6

		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			flag = flag*10 + uint64(c-'0')
		}

		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}