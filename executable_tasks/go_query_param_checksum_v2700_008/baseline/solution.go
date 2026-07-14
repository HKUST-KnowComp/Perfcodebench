package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		parts := strings.Split(row, "&")
		u, _ := strconv.Atoi(strings.TrimPrefix(parts[0], "u="))
		score, _ := strconv.Atoi(strings.TrimPrefix(parts[1], "score="))
		flag, _ := strconv.Atoi(strings.TrimPrefix(parts[2], "flag="))
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
