package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		firstAmp := strings.Index(row, "&")
		u, _ := strconv.Atoi(row[2:firstAmp])
		postFirst := row[firstAmp+1:]
		secondAmp := strings.Index(postFirst, "&")
		score, _ := strconv.Atoi(postFirst[6:secondAmp])
		postSecond := postFirst[secondAmp+1:]
		flag, _ := strconv.Atoi(postSecond[5:])
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}