package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// We need parts[2], parts[3], and parts[4]
		
		var p2, p3, p4 string
		commaCount := 0
		start := 0
		
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					p2 = row[start:i]
				} else if commaCount == 3 {
					p3 = row[start:i]
				} else if commaCount == 4 {
					p4 = row[start:i]
					// We found the start of field 4, but we need the end of it.
					// However, field 4 is the last one we care about in this logic.
				}
				start = i + 1
			}
		}
		// Handle the last field if the loop finished
		// In the original code, parts[4] is the 5th element (index 4)
		// If the row ends after field 4, the loop above won't capture p4 correctly if there's no trailing comma
		// Let's refine the logic to be more robust and faster.
		
		// Re-implementing with a more efficient single-pass approach
		idx := 0
		fieldIdx := 0
		var val2, val3 int
		var isHot bool
		
		// We need to find field 2, 3, and 4
		// Field 0, Field 1, Field 2 (val2), Field 3 (val3), Field 4 (isHot)
		
		currentStart := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				if fieldIdx == 2 {
					val2 = fastAtoi(row[currentStart:i])
				} else if fieldIdx == 3 {
					val3 = fastAtoi(row[currentStart:i])
				} else if fieldIdx == 4 {
					isHot = row[currentStart:i] == "hot"
				}
				fieldIdx++
				currentStart = i + 1
			}
			// Optimization: if we already processed field 4, we can stop scanning this row
			if fieldIdx > 4 {
				break
			}
		}

		v := uint64(val2 + val3 + 25)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func fastAtoi(s string) int {
	if len(s) == 0 {
		return 0
	}
	res := 0
	neg := false
	start := 0
	if s[0] == '-' {
		neg = true
		start = 1
	} else if s[0] == '+' {
		start = 1
	}
	for i := start; i < len(s); i++ {
		c := s[i]
		if c < '0' || c > '9' {
			break
		}
		res = res*10 + int(c-'0')
	}
	if neg {
		return -res
	}
	return res
}
