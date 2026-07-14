package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual split to avoid allocations from strings.Split
		// We need parts[2], parts[3], and parts[4]
		// Format: col0,col1,col2,col3,col4,...
		
		idx := 0
		commaCount := 0
		start := 0
		
		var val2, val3 uint64
		var isHot bool
		
		// We need to find the 3rd, 4th, and 5th fields
		// Field 2 (index 2), Field 3 (index 3), Field 4 (index 4)
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					// Found start of field 2
					val2 = fastParseInt(row[start:i])
				}
				if commaCount == 3 {
					// Found start of field 3
					val3 = fastParseInt(row[start:i])
				}
				start = i + 1
			} else if commaCount == 4 && i == len(row)-1 {
				// Handle case where last field is at the end of string
				// But the loop logic needs to handle the end of string for field 4
			}
			// This manual loop is getting complex, let's simplify the logic
		}
		// Re-implementing cleaner manual scan
		
		// Reset variables for clean scan
		commaCount = 0
		start = 0
		val2 = 0
		val3 = 0
		isHot = false
		
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				commaCount++
				if commaCount == 3 {
					val2 = fastParseInt(row[start:i])
				} else if commaCount == 4 {
					val3 = fastParseInt(row[start:i])
				} else if commaCount == 5 {
					isHot = row[start:i] == "hot"
				}
				start = i + 1
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

func fastParseInt(s string) uint64 {
	if len(s) == 0 {
		return 0
	}
	var res uint64
	for i := 0; i < len(s); i++ {
		res = res*10 + uint64(s[i]-'0')
	}
	return res
}
