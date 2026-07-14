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

func parseRow(s string) (int, int, int) {
	i := 0
	// skip "u="
	i += 2
	u := 0
	for i < len(s) && s[i] >= '0' && s[i] <= '9' {
		u = u*10 + int(s[i]-'0')
		i++
	}
	// skip '&' and "score="
	i++
	i += 6
	score := 0
	for i < len(s) && s[i] >= '0' && s[i] <= '9' {
		score = score*10 + int(s[i]-'0')
		i++
	}
	// skip '&' and "flag="
	i++
	i += 5
	flag := 0
	for i < len(s) && s[i] >= '0' && s[i] <= '9' {
		flag = flag*10 + int(s[i]-'0')
		i++
	}
	return u, score, flag
}
