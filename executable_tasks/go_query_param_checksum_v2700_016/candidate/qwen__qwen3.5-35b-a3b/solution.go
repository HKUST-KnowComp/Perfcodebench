package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Find u=
		uStart := strings.Index(row, "u=")
		uEnd := strings.Index(row[uStart+2:], "&")
		if uEnd == -1 {
			uEnd = len(row) - uStart - 2
		} else {
			uEnd += uStart + 2
		}
		u, _ := strconv.Atoi(row[uStart+2 : uEnd])

		// Find score= after uEnd
		scoreStart := strings.Index(row[uEnd:], "score=")
		scoreStart += uEnd
		scoreEnd := strings.Index(row[scoreStart+6:], "&")
		if scoreEnd == -1 {
			scoreEnd = len(row) - scoreStart - 6
		} else {
			scoreEnd += scoreStart + 6
		}
		score, _ := strconv.Atoi(row[scoreStart+6 : scoreEnd])

		// Find flag= after scoreEnd
		flagStart := strings.Index(row[scoreEnd:], "flag=")
		flagStart += scoreEnd
		flagEnd := strings.Index(row[flagStart+5:], "&")
		if flagEnd == -1 {
			flagEnd = len(row) - flagStart - 5
		} else {
			flagEnd += flagStart + 5
		}
		flag, _ := strconv.Atoi(row[flagStart+5 : flagEnd])

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}