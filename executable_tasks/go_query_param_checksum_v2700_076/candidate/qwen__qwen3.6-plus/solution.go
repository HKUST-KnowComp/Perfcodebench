package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 2
		var u uint64
		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			u = u*10 + uint64(c-'0')
		}
		i += 7
		var score uint64
		for ; i < len(row); i++ {
			c := row[i]
			if c < '0' || c > '9' {
				break
			}
			score = score*10 + uint64(c-'0')
		}
		i += 6
		var flag uint64
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