package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var current int
		var val int
		var parsing bool

		// Manual parsing to avoid allocations and overhead of Split/TrimPrefix
		for i := 0; i < len(row); i++ {
			c := row[i]
			if c >= '0' && c <= '9' {
				val = val*10 + int(c-'0')
				parsing = true
			} else if c == '=' {
				val = 0
			} else if c == '&' || i == len(row)-1 {
				if i == len(row)-1 && c >= '0' && c <= '9' {
					val = val*10 + int(c-'0')
				}
				if current == 0 {
					u = val
				} else if current == 1 {
					score = val
				} else {
					flag = val
				}
				current++
				val = 0
				parsing = false
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}