package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Check for "user"
		userIndex := strings.Index(row, "\"user\":")
		if userIndex != -1 {
			start := userIndex + len("\"user\":")
			// Skip whitespace
			for start < len(row) && (row[start] == ' ' || row[start] == '\t') {
				start++
			}
			// Parse digits
			for start < len(row) && row[start] >= '0' && row[start] <= '9' {
				user = user*10 + int(row[start] - '0')
				start++
			}
		}

		// Check for "score"
		scoreIndex := strings.Index(row, "\"score\":")
		if scoreIndex != -1 {
			start := scoreIndex + len("\"score\":")
			// Skip whitespace
			for start < len(row) && (row[start] == ' ' || row[start] == '\t') {
				start++
			}
			// Parse digits
			for start < len(row) && row[start] >= '0' && row[start] <= '9' {
				score = score*10 + int(row[start] - '0')
				start++
			}
		}

		// Check for "flag"
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