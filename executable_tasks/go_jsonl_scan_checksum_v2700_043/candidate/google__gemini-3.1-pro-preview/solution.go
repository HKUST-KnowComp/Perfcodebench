package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// The baseline splits by comma and checks each part for the keys.
		// We can simulate this by scanning the row and tracking segments.
		start := 0
		for start < len(row) {
			end := strings.IndexByte(row[start:], ',')
			var part string
			if end == -1 {
				part = row[start:]
				start = len(row)
			} else {
				part = row[start : start+end]
				start += end + 1
			}

			// Check for keys in the current segment
			// We use strings.Index for speed as it is highly optimized in Go.
			if strings.Index(part, "\"user\":") != -1 {
				for i := 0; i < len(part); i++ {
					c := part[i]
					if c >= '0' && c <= '9' {
						user = user*10 + int(c-'0')
					}
				}
			} else if strings.Index(part, "\"score\":") != -1 {
				for i := 0; i < len(part); i++ {
					c := part[i]
					if c >= '0' && c <= '9' {
						score = score*10 + int(c-'0')
					}
				}
			} else if strings.Index(part, "\"flag\":true") != -1 {
				flag = true
			}
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