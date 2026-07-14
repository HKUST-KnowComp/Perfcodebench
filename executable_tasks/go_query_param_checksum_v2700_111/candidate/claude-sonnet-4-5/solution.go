package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u := parseU(row)
		score := parseScore(row)
		flag := parseFlag(row)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseU(s string) int {
	i := 2
	for i < len(s) && s[i] != '&' {
		i++
	}
	return atoi(s[2:i])
}

func parseScore(s string) int {
	i := 0
	for i < len(s) && s[i] != '&' {
		i++
	}
	i++
	start := i + 6
	for i < len(s) && s[i] != '&' {
		i++
	}
	return atoi(s[start:i])
}

func parseFlag(s string) int {
	i := 0
	count := 0
	for i < len(s) {
		if s[i] == '&' {
			count++
			if count == 2 {
				return atoi(s[i+6:])
			}
		}
		i++
	}
	return 0
}

func atoi(s string) int {
	n := 0
	for i := 0; i < len(s); i++ {
		n = n*10 + int(s[i]-'0')
	}
	return n
}
