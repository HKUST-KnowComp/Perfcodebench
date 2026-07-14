package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// parse u= (starts at index 2)
		u, i := parseInt(row, 2)
		// skip "&score=" (7 characters)
		i += 7
		score, i := parseInt(row, i)
		// skip "&flag=" (6 characters)
		i += 6
		flag, _ := parseInt(row, i)

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseInt(s string, start int) (int, int) {
	neg := false
	i := start
	if i < len(s) && s[i] == '-' {
		neg = true
		i++
	}
	val := 0
	for i < len(s) && s[i] >= '0' && s[i] <= '9' {
		val = val*10 + int(s[i]-'0')
		i++
	}
	if neg {
		val = -val
	}
	return val, i
}
