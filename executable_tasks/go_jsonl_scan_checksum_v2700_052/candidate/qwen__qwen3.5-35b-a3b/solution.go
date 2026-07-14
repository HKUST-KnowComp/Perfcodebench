package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Parse user
		userIdx := strings.Index(row, "\"user\":")
		if userIdx != -1 {
			start := userIdx + len("\"user\":")
			for i := start; i < len(row); i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
				} else {
					break
				}
			}
		}

		// Parse score
		scoreIdx := strings.Index(row, "\"score\":")
		if scoreIdx != -1 {
			start := scoreIdx + len("\"score\":")
			for i := start; i < len(row); i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
				} else {
					break
				}
			}
		}

		// Check flag
		if strings.Contains(row, "\"flag\":true") {
			flag = true
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}