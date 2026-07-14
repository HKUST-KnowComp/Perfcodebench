package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		
		// Manual scan to avoid strings.Split and strconv.Atoi allocations
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == '&' {
				segment := row[start:i]
				parseParam(segment, &u, &score, &flag)
				start = i + 1
			}
		}
		// Last segment
		parseParam(row[start:], &u, &score, &flag)

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseParam(s string, u, score, flag *int) {
	if len(s) < 3 { return }
	// Identify which parameter we are looking at based on the key
	// u=, score=, flag=
	if s[0] == 'u' && s[1] == '=' {
		*u = fastAtoi(s[2:])
	} else if len(s) > 6 && s[0] == 's' && s[1] == 'c' && s[2] == 'o' && s[3] == 'r' && s[4] == 'e' && s[5] == '=' {
		*score = fastAtoi(s[6:])
	} else if len(s) > 5 && s[0] == 'f' && s[1] == 'l' && s[2] == 'a' && s[3] == 'g' && s[4] == '=' {
		*flag = fastAtoi(s[5:])
	}
}

func fastAtoi(s string) int {
	val := 0
	for i := 0; i < len(s); i++ {
		if s[i] < '0' || s[i] > '9' {
			break
		}
		val = val*10 + int(s[i]-'0')
	}
	return val
}