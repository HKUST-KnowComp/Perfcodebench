package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		pos1 := strings.Index(row, "&")
		u, _ := strconv.Atoi(row[2:pos1])
		pos2 := strings.Index(row[pos1+1:], "&")
		score, _ := strconv.Atoi(row[pos1+1+6 : pos1+1+pos2])
		flag, _ := strconv.Atoi(row[pos1+1+pos2+1+5:])
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}