package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// parts[2] is index 2, parts[3] is index 3, parts[4] is index 4
		
		commaCount := 0
		var start2, start3, start4 int
		var end4 int
		
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					start2 = i + 1
				}
				if commaCount == 3 {
					start3 = i + 1
				}
				if commaCount == 4 {
					start4 = i + 1
					end4 = i
				}
			}
		}
		
		// If row doesn't have enough commas, we skip or handle as per original logic
		// Original logic assumes parts[2], [3], [4] exist.
		
		// Parse parts[2]
		val2 := 0
		foundEnd2 := false
		for i := start2; i < len(row); i++ {
			if row[i] == ',' {
				val2 = val2*10 + int(row[i-1]-'0')
				foundEnd2 = true
				break
			}
			val2 = val2*10 + int(row[i]-'0')
		}
		// Note: The logic above is slightly flawed for general cases but follows the pattern
		// of the original code which used strconv.Atoi on Split results.
		// Let's rewrite the parser more robustly for the specific indices.
		
		// Re-implementing more cleanly:
		idx := 0
		for i := 0; i < 3; i++ {
			for idx < len(row) && row[idx] != ',' {
				idx++
			}
			idx++ // skip comma
		}
		// Now idx is at start of parts[3]
		// Wait, the original code uses parts[2], parts[3], parts[4].
		// parts[0], parts[1], parts[2], parts[3], parts[4]
		// Indices of commas: 0, 1, 2, 3, 4
		// parts[2] is between comma 2 and 3.
		
		// Let's use a more reliable approach for the specific indices required.
		
		// Find comma 2
		c2 := -1
		c3 := -1
		c4 := -1
		count := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				count++
				if count == 2 { c2 = i }
				if count == 3 { c3 = i }
				if count == 4 { c4 = i }
			}
		}
		
		// parts[2] is row[c2+1 : c3]
		b := 0
		for i := c2 + 1; i < c3; i++ {
			b = b*10 + int(row[i]-'0')
		}
		
		// parts[3] is row[c3+1 : c4]
		c := 0
		for i := c3 + 1; i < c4; i++ {
			c = c*10 + int(row[i]-'0')
		}
		
		// parts[4] is row[c4+1 : ]
	hot := false
		for i := c4 + 1; i < len(row); i++ {
			if row[i] == 'h' {
				if i+3 < len(row) && row[i+1] == 'o' && row[i+2] == 't' {
					hot = true
					break
				}
			}
		}
		
		v := uint64(b + c + 24)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
