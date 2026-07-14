package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		parts := strings.Split(row, ",")
		user := 0
		score := 0
		flag := false
		for _, part := range parts {
			if strings.Contains(part, "\"user\":") {
				for _, c := range part {
					if c >= '0' && c <= '9' {
						user = user*10 + int(c-'0')
					}
				}
			} else if strings.Contains(part, "\"score\":") {
				for _, c := range part {
					if c >= '0' && c <= '9' {
						score = score*10 + int(c-'0')
					}
				}
			} else if strings.Contains(part, "\"flag\":true") {
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
