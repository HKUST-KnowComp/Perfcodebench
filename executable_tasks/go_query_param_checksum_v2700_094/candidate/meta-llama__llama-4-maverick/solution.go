package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manually parse the query parameters
		uStart := 2 // len("u=")
		uEnd := indexByte(row, '&')
		scoreStart := uEnd + 7 // uEnd + len("&score=")
		scoreEnd := indexByte(row[scoreStart:], '&') + scoreStart
		flagStart := scoreEnd + 6 // scoreEnd + len("&flag=")

		u, _ := strconv.Atoi(row[uStart:uEnd])
		score, _ := strconv.Atoi(row[scoreStart:scoreEnd])
		flag, _ := strconv.Atoi(row[flagStart:])

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func indexByte(s string, c byte) int {
	for i := 0; i < len(s); i++ {
		if s[i] == c {
			return i
		}
	}
	return -1
}