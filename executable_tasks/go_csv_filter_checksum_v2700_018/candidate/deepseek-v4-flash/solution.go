package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	const (
		initHash uint64 = 1469598103934665603
		mult     uint64 = 1099511628211
	)

	hash := initHash
	for _, row := range rows {
		// Find the positions of the first four commas
		c1 := strings.IndexByte(row, ',')
		c2 := strings.IndexByte(row[c1+1:], ',') + c1 + 1
		c3 := strings.IndexByte(row[c2+1:], ',') + c2 + 1
		c4 := strings.IndexByte(row[c3+1:], ',') + c3 + 1

		// Extract the required fields as substrings (no allocation of a slice)
		b, _ := strconv.Atoi(row[c1+1 : c2])
		c, _ := strconv.Atoi(row[c2+1 : c3])
		hot := row[c3+1:c4] == "hot"

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}

		hash ^= v + 1
		hash *= mult
	}
	return hash
}
