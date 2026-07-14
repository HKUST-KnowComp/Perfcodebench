package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var user uint64
		var score uint64

		if idx := strings.Index(row, "\"user\":"); idx >= 0 {
			user = parseUintAfter(row, idx+7)
		}
		if idx := strings.Index(row, "\"score\":"); idx >= 0 {
			score = parseUintAfter(row, idx+8)
		}
		flag := strings.Index(row, "\"flag\":true") >= 0

		v := (user & 1023) + score
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseUintAfter(s string, i int) uint64 {
	n := len(s)
	for i < n && s[i] == ' ' {
		i++
	}
	if i < n && s[i] == '-' {
		i++
	}
	var v uint64
	for i < n {
		c := s[i]
		if c < '0' || c > '9' {
			break
		}
		v = v*10 + uint64(c-'0')
		i++
	}
	return v
}
