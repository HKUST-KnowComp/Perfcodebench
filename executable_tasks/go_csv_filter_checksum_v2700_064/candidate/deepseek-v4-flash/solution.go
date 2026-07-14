package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// locate first three commas
		i1 := strings.IndexByte(row, ',')
		if i1 < 0 {
			continue
		}
		// second comma
		i2 := strings.IndexByte(row[i1+1:], ',')
		if i2 < 0 {
			continue
		}
		i2 += i1 + 1
		// third comma
		i3 := strings.IndexByte(row[i2+1:], ',')
		if i3 < 0 {
			continue
		}
		i3 += i2 + 1
		// fourth comma (or end) for field 4
		i4 := strings.IndexByte(row[i3+1:], ',')
		var field4 string
		if i4 >= 0 {
			field4 = row[i3+1 : i3+1+i4]
		} else {
			field4 = row[i3+1:]
		}

		b, _ := strconv.Atoi(row[i1+1 : i2])
		c, _ := strconv.Atoi(row[i2+1 : i3])
		hot := field4 == "hot"

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
