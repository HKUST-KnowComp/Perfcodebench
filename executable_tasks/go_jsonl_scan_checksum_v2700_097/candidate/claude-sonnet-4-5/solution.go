package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// Find user field
		if idx := findField(row, "\"user\":"); idx >= 0 {
			user = extractInt(row, idx+7)
		}
		
		// Find score field
		if idx := findField(row, "\"score\":"); idx >= 0 {
			score = extractInt(row, idx+8)
		}
		
		// Find flag field
		if idx := findField(row, "\"flag\":true"); idx >= 0 {
			flag = true
		}
		
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func findField(s, pattern string) int {
	for i := 0; i <= len(s)-len(pattern); i++ {
		match := true
		for j := 0; j < len(pattern); j++ {
			if s[i+j] != pattern[j] {
				match = false
				break
			}
		}
		if match {
			return i
		}
	}
	return -1
}

func extractInt(s string, start int) int {
	num := 0
	for i := start; i < len(s); i++ {
		c := s[i]
		if c >= '0' && c <= '9' {
			num = num*10 + int(c-'0')
		} else if num > 0 {
			break
		}
	}
	return num
}
