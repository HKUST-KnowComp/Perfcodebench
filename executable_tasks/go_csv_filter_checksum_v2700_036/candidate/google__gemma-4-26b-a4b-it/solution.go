package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// We need parts[2], parts[3], and parts[4]
		// parts[0], parts[1], parts[2], parts[3], parts[4]
		
		// Find index of 2nd comma (end of parts[1], start of parts[2])
		idx1 := -1
		idx2 := -1
		idx3 := -1
		idx4 := -1
		
		count := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				count++
				if count == 2 {
					idx2 = i + 1
				} else if count == 3 {
					idx3 = i + 1
				} else if count == 4 {
					idx4 = i + 1
					break
				}
			}
		}

		// If we didn't find enough commas, the row is malformed relative to expectations
		// but based on the baseline, we assume valid input.
		
		// Parse parts[2]
		start2 := idx2
		end2 := idx3
		if end2 == -1 { end2 = len(row) }
		b := 0
		for i := start2; i < end2; i++ {
			b = b*10 + int(row[i]-'0')
		}

		// Parse parts[3]
		start3 := idx3
		end3 := idx4
		if end3 == -1 { end3 = len(row) }
		c := 0
		for i := start3; i < end3; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check parts[4] for "hot"
		start4 := idx4
		// We need to check if the substring starting at start4 is exactly "hot"
		// Note: parts[4] might be the last element, so we check until end of string
		hot := false
		if start4+3 <= len(row) && row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't' {
			// Ensure it's not "hotter" or something else by checking if it's the end or followed by comma
			if start4+3 == len(row) || row[start4+3] == ',' {
				hot = true
			}
		}

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}