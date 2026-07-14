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

		b_str := row[i2+1 : i3]
		c_str := row[i3+1 : i4]
		hot := false
		if len(row)-(i4+1) == 3 && row[i4+1] == 'h' && row[i4+2] == 'o' && row[i4+3] == 't' {
			hot = true
		}

		b, _ := strconv.Atoi(b_str)
		c, _ := strconv.Atoi(c_str)
		v := uint64(b + c + 7)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}