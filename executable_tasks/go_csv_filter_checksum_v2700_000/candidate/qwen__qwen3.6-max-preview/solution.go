package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		c1 := strings.IndexByte(row, ',')
		c2 := strings.IndexByte(row[c1+1:], ',') + c1 + 1
		c3 := strings.IndexByte(row[c2+1:], ',') + c2 + 1
		c4 := strings.IndexByte(row[c3+1:], ',') + c3 + 1

		c5rel := strings.IndexByte(row[c4+1:], ',')
		c5 := len(row)
		if c5rel != -1 {
			c5 = c5rel + c4 + 1
		}

		b, _ := strconv.Atoi(row[c2+1 : c3])
		c, _ := strconv.Atoi(row[c3+1 : c4])
		hot := row[c4+1:c5] == "hot"

		v := uint64(b + c + 7)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}