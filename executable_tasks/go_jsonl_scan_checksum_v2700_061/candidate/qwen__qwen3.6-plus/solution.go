package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		if idx := strings.Index(row, `"user":`); idx != -1 {
			idx += 7
			for idx < len(row) {
				c := row[idx]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
				} else if c == ',' || c == '}' {
					break
				}
				idx++
			}
		}

		if idx := strings.Index(row, `"score":`); idx != -1 {
			idx += 8
			for idx < len(row) {
				c := row[idx]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
				} else if c == ',' || c == '}' {
					break
				}
				idx++
			}
		}

		if strings.Index(row, `"flag":true`) != -1 {
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