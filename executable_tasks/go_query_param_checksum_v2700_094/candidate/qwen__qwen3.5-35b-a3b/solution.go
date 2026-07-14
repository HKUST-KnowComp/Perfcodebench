package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		pos1 := strings.Index(row, "&")
		pos2 := strings.Index(row[pos1+1:], "&") + pos1 + 1
		u_val := row[2:pos1]
		score_val := row[pos1+1+6 : pos2]
		flag_val := row[pos2+1+5 : len(row)]
		u, _ := strconv.Atoi(u_val)
		score, _ := strconv.Atoi(score_val)
		flag, _ := strconv.Atoi(flag_val)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}