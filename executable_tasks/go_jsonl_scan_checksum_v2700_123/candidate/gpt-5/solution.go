package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		if idx := strings.Index(row, "\"user\":"); idx >= 0 {
			start := idx + len("\"user\":")
			end := len(row)
			if p := strings.IndexByte(row[start:], ','); p >= 0 {
				end = start + p
			}
			v := 0
			for i := start; i < end; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					v = v*10 + int(c-'0')
				}
			}
			user = v
		}

		if idx := strings.Index(row, "\"score\":"); idx >= 0 {
			start := idx + len("\"score\":")
			end := len(row)
			if p := strings.IndexByte(row[start:], ','); p >= 0 {
				end = start + p
			}
			v := 0
			for i := start; i < end; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					v = v*10 + int(c-'0')
				}
			}
			score = v
		}

		if strings.Index(row, "\"flag\":true") >= 0 {
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
