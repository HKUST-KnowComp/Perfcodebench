package main

import "strings"

func parsePositiveInt(s string) int {
	var res int
	for _, c := range s {
		res = res*10 + int(c-'0')
	}
	return res
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		firstAmp := strings.IndexByte(row, '&')
		secondAmpRel := strings.IndexByte(row[firstAmp+1:], '&')
		secondAmp := secondAmpRel + firstAmp + 1
		
		u := parsePositiveInt(row[2:firstAmp])
		score := parsePositiveInt(row[firstAmp+7:secondAmp])
		flag := parsePositiveInt(row[secondAmp+6:])
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}