package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		n := len(row)
		// parse u=
		i := 2
		var u uint64
		for i < n && row[i] >= '0' && row[i] <= '9' {
			u = u*10 + uint64(row[i]-'0')
			i++
		}
		// skip "&score="
		i += 7 // 1 for '&', 6 for "score="
		var score uint64
		for i < n && row[i] >= '0' && row[i] <= '9' {
			score = score*10 + uint64(row[i]-'0')
			i++
		}
		// skip "&flag="
		i += 6 // 1 for '&', 5 for "flag="
		var flag uint64
		for i < n && row[i] >= '0' && row[i] <= '9' {
			flag = flag*10 + uint64(row[i]-'0')
			i++
		}
		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}