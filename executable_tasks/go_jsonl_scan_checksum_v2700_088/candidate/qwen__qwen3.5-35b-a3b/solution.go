package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Find "user":
		idx := strings.Index(row, "\"user\":")
		if idx != -1 {
			idx += len("\"user\":")
			// Skip whitespace
			for idx < len(row) && (row[idx] == ' ' || row[idx] == '\t') {
				idx++
			}
			// Parse digits
			for idx < len(row) && row[idx] >= '0' && row[idx] <= '9' {
				user = user*10 + int(row[idx] - '0')
				idx++
			}
		}

		// Find "score":
		idx = strings.Index(row, "\"score\":")
		if idx != -1 {
			idx += len("\"score\":")
			// Skip whitespace
			for idx < len(row) && (row[idx] == ' ' || row[idx] == '\t') {
				idx++
			}
			// Parse digits
			for idx < len(row) && row[idx] >= '0' && row[idx] <= '9' {
				score = score*10 + int(row[idx] - '0')
				idx++
			}
		}

		// Check for "flag":true
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