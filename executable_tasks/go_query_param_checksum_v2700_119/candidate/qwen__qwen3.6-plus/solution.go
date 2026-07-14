package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i1 := strings.IndexByte(row, '&')
		i2 := strings.IndexByte(row[i1+1:], '&') + i1 + 1

		u, _ := strconv.Atoi(row[2:i1])
		score, _ := strconv.Atoi(row[i1+7:i2])
		flag, _ := strconv.Atoi(row[i2+6:])

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}