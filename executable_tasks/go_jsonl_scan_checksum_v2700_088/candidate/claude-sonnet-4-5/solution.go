package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				part := row[start:i]
				start = i + 1
				
				// Check for "user":
				if len(part) > 7 {
					if idx := indexSubstring(part, "\"user\":"); idx >= 0 {
						for j := idx + 7; j < len(part); j++ {
							c := part[j]
							if c >= '0' && c <= '9' {
								user = user*10 + int(c-'0')
							}
						}
						continue
					}
				}
				
				// Check for "score":
				if len(part) > 8 {
					if idx := indexSubstring(part, "\"score\":"); idx >= 0 {
						for j := idx + 8; j < len(part); j++ {
							c := part[j]
							if c >= '0' && c <= '9' {
								score = score*10 + int(c-'0')
							}
						}
						continue
					}
				}
				
				// Check for "flag":true
				if len(part) > 12 {
					if indexSubstring(part, "\"flag\":true") >= 0 {
						flag = true
					}
				}
			}
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

func indexSubstring(s, substr string) int {
	if len(substr) == 0 || len(s) < len(substr) {
		return -1
	}
	first := substr[0]
	for i := 0; i <= len(s)-len(substr); i++ {
		if s[i] == first {
			match := true
			for j := 1; j < len(substr); j++ {
				if s[i+j] != substr[j] {
					match = false
					break
				}
			}
			if match {
				return i
			}
		}
	}
	return -1
}