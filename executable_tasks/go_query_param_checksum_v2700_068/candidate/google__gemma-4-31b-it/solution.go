package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		
		// Manual scan to avoid Split and TrimPrefix allocations
		start := 0
		found := 0
		for i := 0; i < len(row); i++ {
			if row[i] == '&' {
				val := parseValue(row[start:i], found)
				switch found {
				case 0: u = val
				case 1: score = val
				case 2: flag = val
				}
				found++
				start = i + 1
			}
		}
		// Parse the last segment
		if found < 3 {
			val := parseValue(row[start:], found)
			switch found {
			case 0: u = val
			case 1: score = val
			case 2: flag = val
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseValue(s string, index int) int {
	// Skip the key and the '=' sign
	// u=, score=, flag=
	var offset int
	switch index {
	case 0: offset = 2 // "u="
	case 1: offset = 6 // "score="
	case 2: offset = 5 // "flag="
	default: offset = 0
	}
	
	if len(s) <= offset {
		return 0
	}
	
	val := 0
	for i := offset; i < len(s); i++ {
		val = val*10 + int(s[i]-'0')
	}
	return val
}