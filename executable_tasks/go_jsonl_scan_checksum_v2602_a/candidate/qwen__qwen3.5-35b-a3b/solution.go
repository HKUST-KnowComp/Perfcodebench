package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Sum all user values
		start := 0
		for {
			pos := strings.Index(row[start:], "\"user\":")
			if pos == -1 {
				break
			}
			pos += start
			numStart := pos + len("\"user\":")
			for i := numStart; i < len(row); i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
				} else {
					break
				}
			}
			start = pos + 1
		}

		// Sum all score values
		start = 0
		for {
			pos := strings.Index(row[start:], "\"score\":")
			if pos == -1 {
				break
			}
			pos += start
			numStart := pos + len("\"score\":")
			for i := numStart; i < len(row); i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
				} else {
					break
				}
			}
			start = pos + 1
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