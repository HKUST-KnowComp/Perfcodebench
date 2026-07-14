package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 2 // skip "u="

		u := 0
		sign := 1
		if row[i] == '-' {
			sign = -1
			i++
		} else if row[i] == '+' {
			i++
		}
		for ; i < len(row) && row[i] != '&'; i++ {
			u = u*10 + int(row[i]-'0')
		}
		u *= sign

		i += 7 // skip "&score="

		score := 0
		sign = 1
		if row[i] == '-' {
			sign = -1
			i++
		} else if row[i] == '+' {
			i++
		}
		for ; i < len(row) && row[i] != '&'; i++ {
			score = score*10 + int(row[i]-'0')
		}
		score *= sign

		i += 6 // skip "&flag="

		flag := 0
		sign = 1
		if row[i] == '-' {
			sign = -1
			i++
		} else if row[i] == '+' {
			i++
		}
		for ; i < len(row); i++ {
			flag = flag*10 + int(row[i]-'0')
		}
		flag *= sign

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
