package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i1 := strings.Index(row, ",")
		i2 := strings.Index(row[i1+1:], ",") + i1 + 1
		i3 := strings.Index(row[i2+1:], ",") + i2 + 1
		i4 := strings.Index(row[i3+1:], ",") + i3 + 1

		field2 := row[i2+1 : i3]
		field3 := row[i3+1 : i4]
		hot := (len(row) - i4 - 1 == 3) && row[i4+1] == 'h' && row[i4+2] == 'o' && row[i4+3] == 't'

		b, _ := strconv.Atoi(field2)
		c, _ := strconv.Atoi(field3)
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}