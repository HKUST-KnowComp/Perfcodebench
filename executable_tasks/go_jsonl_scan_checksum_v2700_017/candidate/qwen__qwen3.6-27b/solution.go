package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		if i := strings.Index(row, "\"user\":"); i != -1 {
			start := i + 7
			for start < len(row) && row[start] <= ' '; start++
			for start < len(row) && row[start] >= '0' && row[start] <= '9' {
				user = user*10 + int(row[start]-'0')
				start++
			}
		}

		if i := strings.Index(row, "\"score\":"); i != -1 {
			start := i + 7
			for start < len(row) && row[start] <= ' '; start++
			for start < len(row) && row[start] >= '0' && row[start] <= '9' {
				score = score*10 + int(row[start]-'0')
				start++
			}
		}

		if strings.Index(row, "\"flag\":true") != -1 {
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