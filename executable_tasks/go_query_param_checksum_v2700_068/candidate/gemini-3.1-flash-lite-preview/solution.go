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
		
		// Manual parsing to avoid allocations from Split/TrimPrefix
		// Format: u=...&score=...&flag=...
		idx := 0
		for i := 0; i < 3; i++ {
			// Skip key
			for idx < len(row) && row[idx] != '=' {
				idx++
			}
			idx++ // skip '='
			
			// Parse value
			start := idx
			for idx < len(row) && row[idx] != '&' {
				idx++
			}
			val, _ = strconv.Atoi(row[start:idx])
			idx++ // skip '&'
			
			switch i {
			case 0:
				u = val
			case 1:
				score = val
			case 2:
				flag = val
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}