package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		amp1 := strings.Index(row[2:], "&") + 2
		score_start := amp1 + 1 + 6
		amp2 := strings.Index(row[score_start:], "&") + score_start
		flag_start := amp2 + 1 + 5

		u, _ := strconv.Atoi(row[2:amp1])
		score, _ := strconv.Atoi(row[score_start:amp2])
		flag, _ := strconv.Atoi(row[flag_start:])

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}