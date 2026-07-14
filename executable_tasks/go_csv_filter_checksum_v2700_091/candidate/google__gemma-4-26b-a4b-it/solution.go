package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// parts[2] is the 3rd field, parts[3] is the 4th, parts[4] is the 5th
		
		idx := 0
		fieldCount := 0
		var val2, val3 int
		var isHot bool
		
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				fieldCount++
				if fieldCount == 3 {
					val2, _ = strconv.Atoi(row[start:i])
				} else if fieldCount == 4 {
					val3, _ = strconv.Atoi(row[start:i])
				} else if fieldCount == 5 {
					isHot = (row[start:i] == "hot")
				}
				start = i + 1
			}
			// Optimization: if we found all needed fields, we can stop scanning this row
			if fieldCount == 5 {
				break
			}
			i++
		}

		v := uint64(val2 + val3 + 29)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}