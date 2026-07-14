package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// manually parsing CSV to avoid strings.Split allocation
		partStart, partIdx := 0, 0
		var b, c int
		var hot bool
		for i, r := range row {
			if r == ',' {
				switch partIdx {
				case 2:
					b, _ = strconv.Atoi(row[partStart:i])
				case 3:
					c, _ = strconv.Atoi(row[partStart:i])
				case 4:
					hot = row[partStart:i] == "hot"
				}
				partStart = i + 1
				partIdx++
			}
		}
		// process the last part
		if partIdx == 4 {
			hot = row[partStart:] == "hot"
		}
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
