package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		// Locate only the commas needed to extract required fields, avoid full split
		p1 := strings.Index(row, ",")
		p2 := strings.Index(row[p1+1:], ",") + p1 + 1
		p3 := strings.Index(row[p2+1:], ",") + p2 + 1
		p4 := strings.Index(row[p3+1:], ",") + p3 + 1

		// Parse values directly from string slices (zero heap allocation for slices)
		b, _ := strconv.Atoi(row[p2+1:p3])
		c, _ := strconv.Atoi(row[p3+1:p4])

		// Check for "hot" without allocating a new substring
		isHot := len(row) >= p4+4 && row[p4+1] == 'h' && row[p4+2] == 'o' && row[p4+3] == 't'

		// Exact original calculation logic to preserve checksum correctness
		v := uint64(b + c + 15)
		if isHot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}