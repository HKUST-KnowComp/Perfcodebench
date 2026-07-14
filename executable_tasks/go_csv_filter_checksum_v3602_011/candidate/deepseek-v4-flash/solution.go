package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	for _, row := range rows {
		var b, c int
		var hot bool
		state := 0
		start := 0
		rowLen := len(row)
		for i := 0; i <= rowLen; i++ {
			if i == rowLen || row[i] == ',' {
				if state == 2 {
					b, _ = strconv.Atoi(row[start:i])
				} else if state == 3 {
					c, _ = strconv.Atoi(row[start:i])
				} else if state == 4 {
					fieldLen := i - start
					hot = (fieldLen == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't')
				}
				start = i + 1
				state++
			}
		}
		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
