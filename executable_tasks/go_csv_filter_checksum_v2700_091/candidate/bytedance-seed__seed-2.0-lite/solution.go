package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var commas [4]int
		count := 0
		for i := range row {
			if row[i] == ',' {
				commas[count] = i
				count++
				if count == 4 {
					break
				}
			}
		}
		// Parse required integer fields exactly as original
		b, _ := strconv.Atoi(row[commas[1]+1 : commas[2]])
		c, _ := strconv.Atoi(row[commas[2]+1 : commas[3]])
		// Exact byte-level check for "hot" matching original string comparison
		hotPos := commas[3] + 1
		hot := false
		if hotPos+3 <= len(row) {
			if row[hotPos] == 'h' && row[hotPos+1] == 'o' && row[hotPos+2] == 't' {
				if hotPos+3 == len(row) || row[hotPos+3] == ',' {
					hot = true
				}
			}
		}
		// Preserve original hash calculation logic exactly
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}