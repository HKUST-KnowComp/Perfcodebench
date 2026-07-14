package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := 0
		for row[i] != '=' {
			i++
		}
		i++
		u := 0
		for i < len(row) && row[i] >= '0' && row[i] <= '9' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		i++
		for row[i] != '=' {
			i++
		}
		i++
		score := 0
		for i < len(row) && row[i] >= '0' && row[i] <= '9' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		i++
		for row[i] != '=' {
			i++
		}
		i++
		flag := 0
		for i < len(row) && row[i] >= '0' && row[i] <= '9' {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}