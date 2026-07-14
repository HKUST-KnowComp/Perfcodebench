package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manually parsing the query string to avoid allocations
		uStr := row[2:]
		u := 0
		for i := 0; i < len(uStr); i++ {
			if uStr[i] == '&' {
				u, _ = strconv.Atoi(uStr[:i])
				uStr = uStr[i+1:]
				break
			}
		}
		scoreStr := uStr[6:]
		score := 0
		for i := 0; i < len(scoreStr); i++ {
			if scoreStr[i] == '&' {
				score, _ = strconv.Atoi(scoreStr[:i])
				scoreStr = scoreStr[i+1:]
				break
			}
		}
		flag, _ := strconv.Atoi(scoreStr[5:])
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
