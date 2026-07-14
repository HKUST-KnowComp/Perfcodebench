package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// We iterate through the string once to find keys and values.
		// Since the input is 'JSON-like' and split by commas in the baseline,
		// we can scan for the specific key patterns directly.
		
		for i := 0; i < len(row); {
			// Find the next comma or end of string to define a 'part'
			end := i
			for end < len(row) && row[end] != ',' {
				end++
			}
			
			part := row[i:end]
			
			// Check for "user":
			if len(part) >= 8 && part[0:7] == "\"user\":" {
				for j := 7; j < len(part); j++ {
					c := part[j]
					if c >= '0' && c <= '9' {
						user = user*10 + int(c-'0')
					} else if c != ' ' && c != '"' {
						// If we hit a non-digit that isn't space/quote, stop parsing this number
							break
					}
					}
			} else if len(part) >= 9 && part[0:8] == "\"score\":" {
				for j := 8; j < len(part); j++ {
					c := part[j]
					if c >= '0' && c <= '9' {
						score = score*10 + int(c-'0')
					} else if c != ' ' && c != '"' {
							break
						}
					}
			} else if len(part) >= 11 && part[0:11] == "\"flag\":true" {
				flag = true
			}
			
			i = end + 1
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