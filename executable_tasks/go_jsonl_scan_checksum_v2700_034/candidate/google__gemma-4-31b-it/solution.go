package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Single pass scan of the row to avoid strings.Split allocations
		pos := 0
		for pos < len(row) {
			// Find the next key start
			idx := strings.IndexByte(row[pos:], '"')
			if idx == -1 {
				break
			}
			pos += idx

			// Check for "user":
			if strings.HasPrefix(row[pos:], "\"user\":") {
				pos += 8
				for pos < len(row) && (row[pos] >= '0' && row[pos] <= '9') {
					user = user*10 + int(row[pos]-'0')
					pos++
				}
			} else if strings.HasPrefix(row[pos:], "\"score\":") {
				pos += 9
				for pos < len(row) && (row[pos] >= '0' && row[pos] <= '9') {
					score = score*10 + int(row[pos]-'0')
					pos++
				}
			} else if strings.HasPrefix(row[pos:], "\"flag\":true") {
				flag = true
				pos += 13
			} else {
				pos++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}