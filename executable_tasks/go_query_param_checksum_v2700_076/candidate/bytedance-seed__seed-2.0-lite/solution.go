package main

import "strings"

func parsePositiveInt(s string) int {
	var n int
	for _, c := range s {
		n = n*10 + int(c-'0')
	}
	return n
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Locate first delimiter for u values
		amp1 := strings.Index(row, "&")
		u := parsePositiveInt(row[2:amp1])
		
		// Locate second delimiter for score values
		rest1 := row[amp1+1:]
		amp2 := strings.Index(rest1, "&")
		score := parsePositiveInt(rest1[6:amp2])
		
		// Parse flag from remaining suffix
		rest2 := rest1[amp2+1:]
		flag := parsePositiveInt(rest2[5:])
		
		// Original identical hash calculation
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}