package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		c2, c3, c4 := -1, -1, -1
		comma := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				comma++
				if comma == 2 {
					c2 = i
				} else if comma == 3 {
					c3 = i
				} else if comma == 4 {
					c4 = i
					break
				}
			}
		}
		b, _ := strconv.Atoi(row[c2+1 : c3])
		c, _ := strconv.Atoi(row[c3+1 : c4])

		hot := len(row) > c4+3 && row[c4+1] == 'h' && row[c4+2] == 'o' && row[c4+3] == 't' && (c4+4 == len(row) || row[c4+4] == ',')

		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}