package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u, score, flag := parseRow(row)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseRow(row string) (int, int, int) {
	i := 2
	u := 0
	for i < len(row) && row[i] != '&' {
		u = u*10 + int(row[i]-'0')
		i++
	}
	i += 7
	score := 0
	for i < len(row) && row[i] != '&' {
		score = score*10 + int(row[i]-'0')
		i++
	}
	i += 6
	flag := 0
	for i < len(row) {
		flag = flag*10 + int(row[i]-'0')
		i++
	}
	return u, score, flag
}