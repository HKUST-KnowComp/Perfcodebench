package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manually parse the query parameters
		uStart := 2 // len("u=")
		uEnd := strings.IndexByte(row, '&')
		u, _ := strconv.Atoi(row[uStart:uEnd])

		scoreStart := uEnd + 7 // uEnd + len("&score=")
		scoreEnd := strings.IndexByte(row[scoreStart:], '&') + scoreStart
		score, _ := strconv.Atoi(row[scoreStart:scoreEnd])

		flagStart := scoreEnd + 6 // scoreEnd + len("&flag=")
		flag, _ := strconv.Atoi(row[flagStart:])

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
