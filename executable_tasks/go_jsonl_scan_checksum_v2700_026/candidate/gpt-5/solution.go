package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Detect flag
		if strings.Index(row, "\"flag\":true") >= 0 {
			flag = true
		}

		// Parse user
		if pos := strings.Index(row, "\"user\":"); pos >= 0 {
			i := pos + 7 // len("\"user\":")
			n := len(row)
			for i < n {
				c := row[i]
				if c >= '0' && c <= '9' {
					break
				}
				i++
			}
			v := 0
			for i < n {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				v = v*10 + int(c-'0')
				i++
			}
			user = v
		}

		// Parse score
		if pos := strings.Index(row, "\"score\":"); pos >= 0 {
			i := pos + 8 // len("\"score\":")
			n := len(row)
			for i < n {
				c := row[i]
				if c >= '0' && c <= '9' {
					break
				}
				i++
			}
			v := 0
			for i < n {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				v = v*10 + int(c-'0')
				i++
			}
			score = v
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
