package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int
		var hot bool
		commaCount := 0
		start := 0
	scanRow:
		for i := 0; i < len(row); i++ {
			if row[i] != ',' {
				continue
			}
			switch commaCount {
			case 2:
				b, _ = strconv.Atoi(row[start:i])
			case 3:
				c, _ = strconv.Atoi(row[start:i])
			case 4:
				hot = row[start:i] == "hot"
				break scanRow
			}
			commaCount++
			start = i + 1
		}
		// Handle case where 5th field is the last entry in the row (no trailing comma)
		if commaCount == 4 && !hot {
			hot = row[start:] == "hot"
		}
		// Exact same calculation logic as original to preserve checksum
		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}