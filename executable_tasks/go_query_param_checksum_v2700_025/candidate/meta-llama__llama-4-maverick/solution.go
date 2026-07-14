package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manually parse the query parameters to avoid multiple Split and TrimPrefix calls
		uStr := row[strings.IndexByte(row, '=')+1 : strings.IndexByte(row, '&')]
		scoreStr := row[strings.IndexByte(row, 'e')+2 : strings.IndexByte(row, '&', strings.IndexByte(row, 'e')+1)]
		flagStr := row[strings.LastIndexByte(row, '=')+1 :]

		u, _ := strconv.Atoi(uStr)
		score, _ := strconv.Atoi(scoreStr)
		flag, _ := strconv.Atoi(flagStr)

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
