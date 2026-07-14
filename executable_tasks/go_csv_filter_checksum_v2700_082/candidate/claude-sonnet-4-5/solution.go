package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b, c := parseFields(row)
		v := uint64(b + c + 20)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseFields(row string) (int, int) {
	bytes := stringToBytes(row)
	n := len(bytes)
	
	// Skip field 0 and field 1
	commaCount := 0
	i := 0
	for i < n && commaCount < 2 {
		if bytes[i] == ',' {
			commaCount++
		}
		i++
	}
	
	// Parse field 2 (b)
	start := i
	for i < n && bytes[i] != ',' {
		i++
	}
	b := atoi(bytes[start:i])
	i++ // skip comma
	
	// Parse field 3 (c)
	start = i
	for i < n && bytes[i] != ',' {
		i++
	}
	c := atoi(bytes[start:i])
	i++ // skip comma
	
	// Check field 4 for "hot"
	if i+2 < n && bytes[i] == 'h' && bytes[i+1] == 'o' && bytes[i+2] == 't' {
		c += 29
	}
	
	return b, c
}

func atoi(b []byte) int {
	if len(b) == 0 {
		return 0
	}
	n := 0
	neg := false
	start := 0
	if b[0] == '-' {
		neg = true
		start = 1
	}
	for i := start; i < len(b); i++ {
		n = n*10 + int(b[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}

func stringToBytes(s string) []byte {
	return unsafe.Slice(unsafe.StringData(s), len(s))
}
