package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		
		// Find user field
		userIdx := strings.Index(row, "\"user\":")
		if userIdx >= 0 {
			start := userIdx + 7
			for i := start; i < len(row); i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
				} else if user > 0 {
					break
				}
			}
		}
		
		// Find score field
		scoreIdx := strings.Index(row, "\"score\":")
		if scoreIdx >= 0 {
			start := scoreIdx + 8
			for i := start; i < len(row); i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
				} else if score > 0 {
					break
				}
			}
		}
		
		// Find flag field
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
