package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		a := strings.Index(row, "&")
		b := strings.Index(row[a+1:], "&") + a + 1
		u, _ := strconv.Atoi(row[2:a])
		score, _ := strconv.Atoi(row[a+7:b])
		flag, _ := strconv.Atoi(row[b+6:])
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}