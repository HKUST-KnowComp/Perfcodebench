package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b, c, hot := parseRow(row)
		v := uint64(b + c + 10)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseRow(row string) (int, int, bool) {
	bytes := stringToBytes(row)
	var fieldIdx int
	var start int
	var b, c int
	var hot bool
	
	for i := 0; i < len(bytes); i++ {
		if bytes[i] == ',' {
			if fieldIdx == 2 {
				b = atoi(bytes[start:i])
			} else if fieldIdx == 3 {
				c = atoi(bytes[start:i])
			}
			fieldIdx++
			start = i + 1
		}
	}
	
	// Field 4 (hot check) - remaining bytes after last comma
	hot = len(bytes)-start == 3 && bytes[start] == 'h' && bytes[start+1] == 'o' && bytes[start+2] == 't'
	
	return b, c, hot
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
