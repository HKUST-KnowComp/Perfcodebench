package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const userKey = "\"user\":"
	const scoreKey = "\"score\":"
	const flagKey = "\"flag\":true"

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		if i := strings.Index(row, userKey); i != -1 {
			j := i + len(userKey)
			for j < len(row) && (row[j] < '0' || row[j] > '9') {
				j++
			}
			for j < len(row) && row[j] >= '0' && row[j] <= '9' {
				user = user*10 + int(row[j]-'0')
				j++
			}
		}
		if i := strings.Index(row, scoreKey); i != -1 {
			j := i + len(scoreKey)
			for j < len(row) && (row[j] < '0' || row[j] > '9') {
				j++
			}
			for j < len(row) && row[j] >= '0' && row[j] <= '9' {
				score = score*10 + int(row[j]-'0')
				j++
			}
		}
		if strings.Index(row, flagKey) != -1 {
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