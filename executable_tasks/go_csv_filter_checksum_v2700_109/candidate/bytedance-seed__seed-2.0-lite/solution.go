package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Locate only needed comma positions to avoid full row split allocation
		i1 := strings.Index(row, ",")
		i2 := strings.Index(row[i1+1:], ",") + i1 + 1
		i3 := strings.Index(row[i2+1:], ",") + i2 + 1
		i4 := strings.Index(row[i3+1:], ",") + i3 + 1
		// Extract only required fields with zero per-row heap allocations
		b, _ := strconv.Atoi(row[i2+1:i3])
		c, _ := strconv.Atoi(row[i3+1:i4])
		hot := row[i4+1:] == "hot"
		// Original calculation logic preserved exactly for correctness
		v := uint64(b + c + 24)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}