package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		idx := 2 // skip "u="
		u, next := parseUint(row, idx)
		idx = next + 1 + 6 // skip '&' and "score="
		score, next := parseUint(row, idx)
		idx = next + 1 + 5 // skip '&' and "flag="
		flag, _ := parseUint(row, idx)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseUint(s string, start int) (int, int) {
	val := 0
	i := start
	for i < len(s) && s[i] >= '0' && s[i] <= '9' {
		val = val*10 + int(s[i]-'0')
		i++
	}
	return val, i
}