package main

import "strings"

func run(rows []string) uint64 {
	const fnvPrime = uint64(1099511628211)
	const fnvOffset = uint64(1469598103934665603)
	var hash uint64 = fnvOffset
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				part := row[start:i]
				if strings.Contains(part, "\"user\":") {
					for j := 0; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
						}
					}
				} else if strings.Contains(part, "\"score\":") {
					for j := 0; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
						}
					}
				} else if strings.Contains(part, "\"flag\":true") {
					flag = true
				}
				start = i + 1
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