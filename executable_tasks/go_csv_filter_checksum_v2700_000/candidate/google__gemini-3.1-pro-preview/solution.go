package main

import (
	"strings"
)

// run processes rows of CSV data and calculates a checksum.
// It optimizes performance by avoiding strings.Split and using a fast integer parser.
func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		// Find the start of the 3rd field (index 2)
		c1 := strings.IndexByte(row, ',')
		if c1 == -1 {
			continue
		}
		c2 := strings.IndexByte(row[c1+1:], ',')
		if c2 == -1 {
			continue
		}
		start2 := c1 + c2 + 2

		// Find the end of the 3rd field / start of 4th
		c3 := strings.IndexByte(row[start2:], ',')
		if c3 == -1 {
			continue
		}
		end2 := start2 + c3
		start3 := end2 + 1

		// Find the end of the 4th field / start of 5th
		c4 := strings.IndexByte(row[start3:], ',')
		if c4 == -1 {
			continue
		}
		end3 := start3 + c4
		start4 := end3 + 1

		// Find the end of the 5th field
		end4 := strings.IndexByte(row[start4:], ',')
		if end4 == -1 {
			end4 = len(row)
		} else {
			end4 += start4
		}

		// Parse integers b and c manually for speed
		var b int
		for i := start2; i < end2; i++ {
			b = b*10 + int(row[i]-'0')
		}

		var c int
		for i := start3; i < end3; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check if the 5th field is "hot"
		hot := (end4-start4 == 3) && row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't'

		v := uint64(b + c + 7)
		if hot {
			v += 29
		}

		hash ^= v + 1
		hash *= prime
	}
	return hash
}