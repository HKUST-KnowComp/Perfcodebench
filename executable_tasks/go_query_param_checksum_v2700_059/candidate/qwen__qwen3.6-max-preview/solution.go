package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		n := len(row)
		i := 2
		u := 0
		for i < n {
			c := row[i] - '0'
			if c > 9 {
				break
			}
			u = u*10 + int(c)
			i++
		}

		i += 7
		score := 0
		for i < n {
			c := row[i] - '0'
			if c > 9 {
				break
			}
			score = score*10 + int(c)
			i++
		}

		i += 6
		flag := 0
		for i < n {
			c := row[i] - '0'
			if c > 9 {
				break
			}
			flag = flag*10 + int(c)
			i++
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}