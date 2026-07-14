package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	const prime = 1099511628211
	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		// Find positions of the first four commas (fields 0-4)
		c1 := strings.IndexByte(row, ',')
		if c1 < 0 {
			continue
		}
		c2 := strings.IndexByte(row[c1+1:], ',')
		if c2 < 0 {
			continue
		}
		c2 += c1 + 1
		c3 := strings.IndexByte(row[c2+1:], ',')
		if c3 < 0 {
			continue
		}
		c3 += c2 + 1
		c4 := strings.IndexByte(row[c3+1:], ',')
		if c4 < 0 {
			continue
		}
		c4 += c3 + 1

		// End of field4 (may be last field)
		next := strings.IndexByte(row[c4+1:], ',')
		end := c4 + 1
		if next >= 0 {
			end += next
		} else {
			end = len(row)
		}

		// Extract substrings (no allocation, just string headers)
		field2 := row[c2+1 : c3]
		field3 := row[c3+1 : c4]
		field4 := row[c4+1 : end]

		b, _ := strconv.Atoi(field2)
		c, _ := strconv.Atoi(field3)

		v := uint64(b + c + 16)
		if field4 == "hot" {
			v += 29
		}

		hash ^= v + 1
		hash *= prime
	}

	return hash
}
