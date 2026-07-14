package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var user, score int
		var flag bool
		start := 0
		rowLen := len(row)
		for start < rowLen {
			nextComma := strings.Index(row[start:], ",")
			var part string
			if nextComma == -1 {
				part = row[start:]
				start = rowLen
			} else {
				part = row[start : start+nextComma]
				start += nextComma + 1
			}
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