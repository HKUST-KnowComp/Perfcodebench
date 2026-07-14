package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// Single pass scan to avoid strings.Split and multiple strings.Contains calls
		i := 0
		n := len(row)
		for i < n {
			// Find the start of a potential key
			start := i
			for i < n && row[i] != '"' && row[i] != ',' {
				i++
			}
			
			if i < n && row[i] == '"' {
				// We found a quote, let's see if it's one of our keys
				keyStart := i
				i++ // skip opening quote
				for i < n && row[i] != '"' {
					i++
				}
				keyEnd := i
				i++ // skip closing quote
				
				// Check if this segment contains our target keys
				// We look for the pattern "key":
				// Instead of full string comparison, we check the immediate vicinity
				// to find the value following the colon.
				
				// Fast check for keys
				foundKey := false
				var targetKey string
				
				// Check for "user":
				if keyEnd-keyStart >= 6 && row[keyStart:keyEnd] == "user" {
					targetKey = "user"
					foundKey = true
				} else if keyEnd-keyStart >= 5 && row[keyStart:keyEnd] == "score" {
					targetKey = "score"
					foundKey = true
				} else if keyEnd-keyStart >= 4 && row[keyStart:keyEnd] == "flag" {
					targetKey = "flag"
					foundKey = true
				}

				if foundKey {
					// Look for the colon and the value
					// The value starts after the colon and optional whitespace
					searchIdx := keyEnd
					for searchIdx < n && (row[searchIdx] == ':' || row[searchIdx] == ' ' || row[searchIdx] == '"') {
						searchIdx++
					}
					
					if targetKey == "user" || targetKey == "score" {
						val := 0
						for searchIdx < n && row[searchIdx] >= '0' && row[searchIdx] <= '9' {
							val = val*10 + int(row[searchIdx]-'0')
							searchIdx++
						}
						if targetKey == "user" {
							user = val
						} else {
							score = val
						}
					} else if targetKey == "flag" {
						// Check for "flag":true
						// The original code used strings.Contains(part, "\"flag\":true")
						// We check if the value following the key is 'true'
						if searchIdx+4 <= n && row[searchIdx:searchIdx+4] == "true" {
							flag = true
						}
					}
					i = searchIdx
				} else {
					i = keyEnd
				}
			}
			
			// Move to next part
			for i < n && row[i] != ',' {
				i++
			}
				if i < n && row[i] == ',' {
					i++
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