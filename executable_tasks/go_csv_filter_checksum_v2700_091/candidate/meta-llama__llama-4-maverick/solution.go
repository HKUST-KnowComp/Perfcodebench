package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// manually parsing CSV to avoid memory allocation
		parts := make([]int, 5)
		idx := 0
		partStart := 0
		for i, r := range row {
			if r == ',' {
				parts[idx] = partStart
				idx++
				partStart = i + 1
			}
		}
		parts[4] = partStart

		b := atoi(row[parts[2]:parts[3]])
		c := atoi(row[parts[3]:parts[4]])
		hot := row[parts[4]+1] == 'h' && row[parts[4]+2] == 'o' && row[parts[4]+3] == 't'
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func atoi(s string) int {
	n := 0
	for _, r := range s {
		n = n*10 + int(r-'0')
	}
	return n
}