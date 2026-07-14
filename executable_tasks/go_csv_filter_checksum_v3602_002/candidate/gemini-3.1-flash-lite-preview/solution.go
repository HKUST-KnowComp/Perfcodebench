package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual parsing to avoid strings.Split allocations
		var parts [5]string
		start := 0
		idx := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				parts[idx] = row[start:i]
				start = i + 1
				idx++
			}
		}
		parts[4] = row[start:]

		b, _ := strconv.Atoi(parts[2])
		c, _ := strconv.Atoi(parts[3])
		
		// Faster string comparison for "hot"
		hot := false
		if len(parts[4]) == 3 && parts[4][0] == 'h' && parts[4][1] == 'o' && parts[4][2] == 't' {
			hot = true
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}