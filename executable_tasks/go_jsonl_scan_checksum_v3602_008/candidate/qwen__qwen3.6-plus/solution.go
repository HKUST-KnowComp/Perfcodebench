package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		if i := strings.Index(row, `"user":`); i != -1 {
			i += 7
			for i < len(row) {
				c := row[i]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
					i++
				} else {
					break
				}
			}
		}

		if i := strings.Index(row, `"score":`); i != -1 {
			i += 8
			for i < len(row) {
				c := row[i]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
					i++
				} else {
					break
				}
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