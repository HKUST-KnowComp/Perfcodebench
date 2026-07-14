package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u := parseValue(row, 2)
		score := parseValue(row, findNext(row, 2))
		flag := parseValue(row, findNext(row, findNext(row, 2)))
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func findNext(s string, start int) int {
	for i := start; i < len(s); i++ {
		if s[i] == '&' {
			return i + 1
		}
	}
	return len(s)
}

func parseValue(s string, start int) int {
	for i := start; i < len(s); i++ {
		if s[i] == '=' {
			i++
			val := 0
			for i < len(s) && s[i] >= '0' && s[i] <= '9' {
				val = val*10 + int(s[i]-'0')
				i++
			}
			return val
		}
	}
	return 0
}
