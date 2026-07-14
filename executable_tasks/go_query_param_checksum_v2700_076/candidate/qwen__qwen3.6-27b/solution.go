package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i1 := strings.IndexByte(row, '&')
		i2 := strings.IndexByte(row[i1+1:], '&') + i1 + 1

		u := uint64(0)
		for i := 2; i < i1; i++ {
			u = u*10 + uint64(row[i]-'0')
		}

		score := uint64(0)
		for i := i1 + 7; i < i2; i++ {
			score = score*10 + uint64(row[i]-'0')
		}

		flag := uint64(0)
		for i := i2 + 6; i < len(row); i++ {
			flag = flag*10 + uint64(row[i]-'0')
		}

		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}