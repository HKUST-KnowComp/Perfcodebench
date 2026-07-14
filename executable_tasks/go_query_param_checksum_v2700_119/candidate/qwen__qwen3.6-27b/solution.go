package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		l := len(row)
		i1 := 0
		for i1 < l && row[i1] != '&' {
			i1++
		}
		u := 0
		for i := 2; i < i1; i++ {
			u = u*10 + int(row[i]-'0')
		}

		i2 := i1 + 1
		for i2 < l && row[i2] != '&' {
			i2++
		}
		score := 0
		for i := i1 + 7; i < i2; i++ {
			score = score*10 + int(row[i]-'0')
		}

		flag := 0
		for i := i2 + 7; i < l; i++ {
			flag = flag*10 + int(row[i]-'0')
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}