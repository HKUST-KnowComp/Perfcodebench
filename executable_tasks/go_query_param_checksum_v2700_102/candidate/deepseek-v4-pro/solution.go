package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		n := len(row)
		// parse u
		i := 2 // skip "u="
		u := 0
		for i < n && row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		// skip "&score="
		i += 7
		score := 0
		for i < n && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		// skip "&flag="
		i += 6
		flag := 0
		for i < n {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
