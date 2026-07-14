package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		i := 0
		// parse u
		i += 2 // skip "u="
		var u int
		neg := false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for i < len(row) && row[i] >= '0' && row[i] <= '9' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		if neg {
			u = -u
		}
		// skip "&score="
		i += 7
		// parse score
		var score int
		neg = false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for i < len(row) && row[i] >= '0' && row[i] <= '9' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		if neg {
			score = -score
		}
		// skip "&flag="
		i += 6
		// parse flag
		var flag int
		neg = false
		if i < len(row) && row[i] == '-' {
			neg = true
			i++
		}
		for i < len(row) && row[i] >= '0' && row[i] <= '9' {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		if neg {
			flag = -flag
		}
		// compute hash step
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
