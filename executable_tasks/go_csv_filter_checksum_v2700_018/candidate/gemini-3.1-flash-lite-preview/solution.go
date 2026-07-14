package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int
		var hot bool
		
		// Manual parsing to avoid strings.Split allocations
		commaCount := 0
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				segment := row[start:i]
				switch commaCount {
				case 2:
					b, _ = strconv.Atoi(segment)
				case 3:
					c, _ = strconv.Atoi(segment)
				case 4:
					hot = (segment == "hot")
				}
				commaCount++
				start = i + 1
			}
		}
		// Handle last field
		if row[start:] == "hot" {
			hot = true
		}

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= (v + 1)
		hash *= 1099511628211
	}
	return hash
}