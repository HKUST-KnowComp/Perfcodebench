package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i1 := strings.IndexByte(row, ',')
		i2 := strings.IndexByte(row[i1+1:], ',') + i1 + 1
		i3 := strings.IndexByte(row[i2+1:], ',') + i2 + 1
		i4 := strings.IndexByte(row[i3+1:], ',') + i3 + 1

		b, _ := strconv.Atoi(row[i2+1 : i3])
		c, _ := strconv.Atoi(row[i3+1 : i4])
		
		hot := len(row) > i4+3 && row[i4+1:i4+4] == "hot"
		
		v := uint64(b + c + 10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
