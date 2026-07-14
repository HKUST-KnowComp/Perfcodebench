package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// We scan the row manually to avoid the overhead of strings.Split and multiple Contains calls.
		// Since the input is JSONL-like and comma-separated, we look for the specific key patterns.
		for i := 0; i < len(row); { 
			// Find the next comma or end of string to isolate a part
			end := i
			for end < len(row) && row[end] != ',' {
				end++
			}
			
			part := row[i:end]
			if len(part) > 0 {
				// Check for "user":
				if len(part) >= 8 && part[0:8] == "\"user\":" {
					for j := 8; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
						} else if c != ' ' && c != '"' && c != ',' {
							// Stop if we hit a non-digit that isn't part of the JSON structure
								break
						}
					}
				} else if len(part) >= 8 && part[0:8] == "\"score\":" {
					for j := 8; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
						} else if c != ' ' && c != '"' && c != ',' {
								break
						}
					}
				} else if len(part) >= 11 && part[0:11] == "\"flag\":true" {
					flag = true
				}
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