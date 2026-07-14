package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int
		hot := false
		comma := 0
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				comma++
				switch comma {
				case 3:
					b, _ = strconv.Atoi(row[start:i])
				case 4:
					c, _ = strconv.Atoi(row[start:i])
				case 5:
					hot = row[start:i] == "hot"
					goto done
				}
				start = i + 1
			}
		}
		if comma == 4 {
			hot = row[start:] == "hot"
		}
	done:
		v := uint64(b + c + 24)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
