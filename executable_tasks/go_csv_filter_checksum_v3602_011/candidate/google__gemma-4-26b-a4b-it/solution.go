package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// We need parts[2], parts[3], and parts[4]
		// parts[0], parts[1], parts[2], parts[3], parts[4]
		
		// Manual scan to find commas and extract values without allocating slices
		commaCount := 0
		start := 0
		
		var b, c uint64
		var hot bool
		
		// We iterate through the string to find the 3rd, 4th, and 5th fields
		// Field 2 (index 2), Field 3 (index 3), Field 4 (index 4)
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					// Found start of field 2
					start = i + 1
				} else if commaCount == 3 {
					// Found start of field 3
					// First, parse field 2
					for j := start; j < i; j++ {
						b = b*10 + uint64(row[j]-'0')
					}
					start = i + 1
				} else if commaCount == 4 {
					// Found start of field 4
					// First, parse field 3
					for j := start; j < i; j++ {
						c = c*10 + uint64(row[j]-'0')
					}
					start = i + 1
				}
			}
			
			// Handle the last field (index 4)
			if i == len(row)-1 && commaCount == 4 {
				hot = true
				for j := start; j < len(row); j++ {
					if row[j] == 'h' {
						hot = true
						break
					}
					if row[j] != 'h' && row[j] != 'o' && row[j] != 't' {
						// This is a simplified check for "hot"
						// but since we know the format, we just check the first char
						}
					}
				// More robust check for "hot"
					if len(row)-start >= 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					} else {
						hot = false
					}
				}
		}
		
		// If the loop finished and we didn't find enough commas, the row is malformed.
		// However, the original code used strings.Split which handles this. 
		// Let's use a slightly more robust approach for the fields.
		
		// Re-implementing the loop logic to be cleaner and safer
		commaCount = 0
		start = 0
	b = 0
	c = 0
	hot = false
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				commaCount++
				if commaCount == 3 {
					// Field 2
					for j := start; j < i; j++ {
							if row[j] >= '0' && row[j] <= '9' {
								b = b*10 + uint64(row[j]-'0')
							}
						}
					} else if commaCount == 4 {
					// Field 3
					for j := start; j < i; j++ {
							if row[j] >= '0' && row[j] <= '9' {
							c = c*10 + uint64(row[j]-'0')
						}
						}
					} else if commaCount == 5 {
					// Field 4
					if i-start >= 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
					}
				start = i + 1
				}
		}

		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}