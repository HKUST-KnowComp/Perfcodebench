package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		parts := make([][]byte, 5)
		idx := 0
		start := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				parts[idx] = []byte(row[start:i])
				idx++
				start = i + 1
			}
		}
		parts[idx] = []byte(row[start:])
		b, _ := strconv.Atoi(string(parts[2]))
		c, _ := strconv.Atoi(string(parts[3]))
		hot := parts[4][0] == 'h' && parts[4][1] == 'o' && parts[4][2] == 't'
		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
