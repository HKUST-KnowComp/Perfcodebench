package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		parts := strings.Split(row, ",")
		b, _ := strconv.Atoi(parts[2])
		c, _ := strconv.Atoi(parts[3])
		hot := parts[4] == "hot"
		v := uint64(b + c + 10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
