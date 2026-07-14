package main

import (
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		pos1 := strings.Index(row, "&")
		pos2 := strings.Index(row[pos1+1:], "&") + pos1 + 1

		u_str := row[2 : pos1]
		score_str := row[pos1+1+6 : pos2]
		flag_str := row[pos2+1+5 : ]

		u := parseNum(u_str)
		score := parseNum(score_str)
		flag := parseNum(flag_str)

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseNum(s string) int {
	var num int
	for i := 0; i < len(s); i++ {
		num = num*10 + int(s[i] - '0')
	}
	return num
}