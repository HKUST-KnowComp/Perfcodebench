package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i0 := strings.IndexByte(row, ',')
		i1 := strings.IndexByte(row[i0+1:], ',') + i0 + 1
		i2 := strings.IndexByte(row[i1+1:], ',') + i1 + 1
		i3 := strings.IndexByte(row[i2+1:], ',') + i2 + 1

		b, _ := strconv.Atoi(row[i1+1 : i2])
		c, _ := strconv.Atoi(row[i2+1 : i3])

		hot := i3+4 <= len(row) && row[i3+1] == 'h' && row[i3+2] == 'o' && row[i3+3] == 't' && (i3+4 == len(row) || row[i3+4] == ',')

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}