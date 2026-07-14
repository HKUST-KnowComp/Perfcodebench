package main

import "strings"

func run(rows []string) uint64 {
	const (
		fnvOffset uint64 = 1469598103934665603
		fnvPrime  uint64 = 1099511628211
		userKey         = "\"user\":" 
		scoreKey        = "\"score\":" 
		flagStr         = "\"flag\":true"
		keyLen          = 7
	)
	hash := fnvOffset
	for _, row := range rows {
		var user, score int
		flag := strings.Contains(row, flagStr)

		if userIdx := strings.Index(row, userKey); userIdx != -1 {
			i := userIdx + keyLen
			for i < len(row) && row[i] >= '0' && row[i] <= '9' {
				user = user*10 + int(row[i]-'0')
				i++
			}
		}

		if scoreIdx := strings.Index(row, scoreKey); scoreIdx != -1 {
			i := scoreIdx + keyLen
			for i < len(row) && row[i] >= '0' && row[i] <= '9' {
				score = score*10 + int(row[i]-'0')
				i++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}