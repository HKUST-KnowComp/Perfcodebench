package main

import (
	"strings"
)

func parseInt(s string, start, end int) int {
	nn := 0
	for i := start; i < end; i++ {
		nn = nn*10 + int(s[i]-'0')
	}
	return nn
}

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	const fnvPrime = uint64(1099511628211)
	for _, row := range rows {
		firstAmp := strings.Index(row, "&")
		scoreSub := row[firstAmp+1:]
		secondAmp := strings.Index(scoreSub, "&") + firstAmp + 1
		flagSub := row[secondAmp+1:]
		thirdAmpRaw := strings.Index(flagSub, "&")
		var thirdAmp int
		if thirdAmpRaw == -1 {
			thirdAmp = len(row)
		} else {
			thirdAmp = thirdAmpRaw + secondAmp + 1
		}
		u := parseInt(row, 2, firstAmp)
		score := parseInt(row, firstAmp+7, secondAmp)
		flag := parseInt(row, secondAmp+6, thirdAmp)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}