package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// find 3rd field (index 2)
		i := 0
		start := 0
		for ; i < 2; i++ {
			for start < len(row) && row[start] != ',' {
				start++
			}
			start++ // skip comma
		}
		end := start
		for end < len(row) && row[end] != ',' {
			end++
		}
		b, _ := strconv.Atoi(row[start:end])

		// 4th field (index 3)
		start = end + 1
		end = start
		for end < len(row) && row[end] != ',' {
			end++
		}
		c, _ := strconv.Atoi(row[start:end])

		// 5th field (index 4) for "hot"
		start = end + 1
		end = start
		for end < len(row) && row[end] != ',' {
			end++
		}
		hot := end-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't'

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}