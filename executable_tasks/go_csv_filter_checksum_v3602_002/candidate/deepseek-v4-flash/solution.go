package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	const (
		prime = 1099511628211
		init  = 1469598103934665603
		hotStr = "hot"
	)
	hash := uint64(init)

	for _, row := range rows {
		n := len(row)
		fieldCount := 0
		var field2Start, field2End int
		var field3Start, field3End int
		var field4Start int

		// Scan commas to locate fields 2, 3, and 4 (0-indexed)
		for i := 0; i < n; i++ {
			if row[i] == ',' {
				fieldCount++
				switch fieldCount {
				case 2:
					field2Start = i + 1
				case 3:
					field2End = i
					field3Start = i + 1
				case 4:
					field3End = i
					field4Start = i + 1
					break
				}
			}
		}

		// Extract substrings (no allocation, shares underlying array)
		field2 := row[field2Start:field2End]
		field3 := row[field3Start:field3End]
		field4 := row[field4Start:]

		b, _ := strconv.Atoi(field2)
		c, _ := strconv.Atoi(field3)
		hot := field4 == hotStr

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}

		hash ^= v + 1
		hash *= prime
	}

	return hash
}