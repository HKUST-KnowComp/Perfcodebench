package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		if idx := strings.Index(row, `"user":`); idx >= 0 {
			i := idx + 7
			for i < len(row) && (row[i] < '0' || row[i] > '9') {
				i++
			}
			for i < len(row) && row[i] >= '0' && row[i] <= '9' {
				user = user*10 + int(row[i]-'0')
				i++
			}
		}
		if idx := strings.Index(row, `"score":`); idx >= 0 {
			i := idx + 8
			for i < len(row) && (row[i] < '0' || row[i] > '9') {
				i++
			}
			for i < len(row) && row[i] >= '0' && row[i] <= '9' {
				score = score*10 + int(row[i]-'0')
				i++
			}
		}
		if strings.Contains(row, `"flag":true`) {
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