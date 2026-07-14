package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// parts[2] is the 3rd field, parts[3] is the 4th, parts[4] is the 5th
		
		idx := 0
		commaCount := 0
		
		var valB, valC uint64
		var isHot bool
		
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					// Start of parts[2]
					start := i + 1
					for i+1 < len(row) && row[i+1] != ',' {
						i++
					}
					// Parse parts[2]
					for j := start; j <= i; j++ {
						if row[j] >= '0' && row[j] <= '9' {
							valB = valB*10 + uint64(row[j]-'0')
						}
					}
				} else if commaCount == 3 {
					// Start of parts[3]
					start := i + 1
					for i+1 < len(row) && row[i+1] != ',' {
						i++
					}
					// Parse parts[3]
					for j := start; j <= i; j++ {
						if row[j] >= '0' && row[j] <= '9' {
							valC = valC*10 + uint64(row[j]-'0')
						}
					}
				} else if commaCount == 4 {
					// Start of parts[4]
					start := i + 1
					// Check if parts[4] is "hot"
					// We need to check if the substring from start to end is "hot"
					// Since it's the last field in the logic, we check until end of string
					end := len(row)
					if len(row)-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						isHot = true
					}
					// Break loop as we reached the field we care about
					break
				}
			} else if commaCount == 4 {
				// Handle case where row ends exactly after parts[4] without trailing comma
				// This is covered by the loop logic if we check the end of string
			}
		}
		
		// Re-check for the case where the loop finishes and we haven't processed parts[4]
		// if the row doesn't end with a comma (which is standard).
		// The logic above handles the 'hot' check inside the comma loop or via the end of string.
		// Let's refine the 'hot' check to be more robust.
		
		// Re-calculating logic for clarity and correctness in a single pass
		var b, c uint64
		var hot bool
		commas := 0
		fieldStart := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				field := row[fieldStart:i]
				if commas == 2 {
					for j := 0; j < len(field); j++ {
						if field[j] >= '0' && field[j] <= '9' {
							b = b*10 + uint64(field[j]-'0')
						} else { break }
						}
				} else if commas == 3 {
					for j := 0; j < len(field); j++ {
						if field[j] >= '0' && field[j] <= '9' {
							c = c*10 + uint64(field[j]-'0')
						} else { break }
						}
				} else if commas == 4 {
					hot = (field == "hot")
					break
				}
				commas++
				fieldStart = i + 1
			}
		}

		v := b + c + 16
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}