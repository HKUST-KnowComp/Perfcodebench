package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var user, score int
		var flag bool
		for i := 0; i < len(row); {
			j := i
			for j < len(row) && row[j] != ',' {
				j++
			}
			part := row[i:j]
			if strings.HasPrefix(part, "\"user\":") {
				start := len("\"user\":")
				for k := start; k < len(part); k++ {
					c := part[k]
					if c >= '0' && c <= '9' {
						user = user*10 + int(c - '0')
					} else {
						break
					}
				}
			} else if strings.HasPrefix(part, "\"score\":") {
				start := len("\"score\":")
				for k := start; k < len(part); k++ {
					c := part[k]
					if c >= '0' && c <= '9' {
						score = score*10 + int(c - '0')
					} else {
						break
					}
				}
			} else if strings.HasPrefix(part, "\"flag\":true") {
				flag = true
			}
			i = j + 1
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