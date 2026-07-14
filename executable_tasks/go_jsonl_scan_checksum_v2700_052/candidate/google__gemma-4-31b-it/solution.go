package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Scan the row once to find the required fields
		// We look for the keys and then parse the values immediately following them
		for i := 0; i < len(row); {
			if i+7 <= len(row) && row[i:i+7] == "\"user\":" {
				i += 7
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			} else if i+8 <= len(row) && row[i:i+8] == "\"score\":" {
				i += 8
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
			}
			} else if i+13 <= len(row) && row[i:i+13] == "\"flag\":true" {
				flag = true
				i += 13
			} else {
				i++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}