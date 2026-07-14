package main

import (
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i1 := strings.IndexByte(row, ',')
		if i1 == -1 {
			continue
		}
		i2 := strings.IndexByte(row[i1+1:], ',')
		if i2 == -1 {
			continue
		}
		i2 += i1 + 1

		i3 := strings.IndexByte(row[i2+1:], ',')
		if i3 == -1 {
			continue
		}
		i3 += i2 + 1

		i4 := strings.IndexByte(row[i3+1:], ',')
		if i4 == -1 {
			continue
		}
		i4 += i3 + 1

		b := 0
		for j := i2 + 1; j < i3; j++ {
			b = b*10 + int(row[j]-'0')
		}

		c := 0
		for j := i3 + 1; j < i4; j++ {
			c = c*10 + int(row[j]-'0')
		}

		hot := row[i4+1:] == "hot"

		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
