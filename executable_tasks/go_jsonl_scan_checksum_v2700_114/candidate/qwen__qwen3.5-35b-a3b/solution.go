package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var user, score uint64
		var flag bool

		// Find "user": and parse the number
		if idx := strings.Index(row, "\"user\":"); idx != -1 {
			start := idx + len("\"user\":")
			for start < len(row) && row[start] >= '0' && row[start] <= '9' {
				user = user*10 + uint64(row[start] - '0')
				start++
			}
		}

		// Find "score": and parse the number
		if idx := strings.Index(row, "\"score\":"); idx != -1 {
			start := idx + len("\"score\":")
			for start < len(row) && row[start] >= '0' && row[start] <= '9' {
				score = score*10 + uint64(row[start] - '0')
				start++
			}
		}

		// Check for "flag":true
		if strings.Contains(row, "\"flag\":true") {
			flag = true
		}

		v := (user & 1023) + score
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}