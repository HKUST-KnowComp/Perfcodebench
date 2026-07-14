package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		parts := make([]int, 5)
		idx := 0
		start := 0
		for i, r := range row {
			if r == ',' {
				parts[idx] = start
				idx++
				start = i + 1
			}
		}
		parts[idx] = start

		b, _ := strconv.Atoi(row[parts[2]:parts[3]])
		c, _ := strconv.Atoi(row[parts[3]:parts[4]])
		hot := row[parts[4]] == 'h'
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
