package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		
		// Manual scan to avoid Split and TrimPrefix allocations
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == '&' {
				segment := row[start:i]
				parseValue(segment, &u, &score, &flag)
				start = i + 1
			}
		}
		// Last segment
		parseValue(row[start:], &u, &score, &flag)
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseValue(segment string, u, score, flag *int) {
	if len(segment) == 0 {
		return
	}
	// Identify which field we are parsing based on the prefix
	// u=, score=, flag=
	if segment[0] == 'u' && len(segment) > 2 && segment[1] == '=' {
		*u = fastAtoi(segment[2:])
	} else if len(segment) > 6 && segment[0] == 's' && segment[1] == 'c' && segment[2] == 'o' && segment[3] == 'r' && segment[4] == 'e' && segment[5] == '=' {
		*score = fastAtoi(segment[6:])
	} else if len(segment) > 5 && segment[0] == 'f' && segment[1] == 'l' && segment[2] == 'a' && segment[3] == 'g' && segment[4] == '=' {
		*flag = fastAtoi(segment[5:])
	}
}

func fastAtoi(s string) int {
	var res int
	for i := 0; i < len(s); i++ {
		res = res*10 + int(s[i]-'0')
	}
	return res
}