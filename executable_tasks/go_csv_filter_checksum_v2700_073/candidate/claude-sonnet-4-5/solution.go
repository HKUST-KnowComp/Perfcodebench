package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b, c := parseFields(row)
		v := uint64(b + c + 11)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseFields(row string) (int, int) {
	bytes := stringToBytes(row)
	var commaCount int
	var start int
	var b, c int
	var hot bool
	
	for i := 0; i < len(bytes); i++ {
		if bytes[i] == ',' {
			commaCount++
			if commaCount == 2 {
				b = atoi(bytes[start:i])
				start = i + 1
			} else if commaCount == 3 {
				c = atoi(bytes[start:i])
				start = i + 1
			} else if commaCount == 4 {
				hot = i-start == 3 && bytes[start] == 'h'
				break
			} else if commaCount < 2 {
				start = i + 1
			}
		}
	}
	
	if hot {
		return b, c + 29
	}
	return b, c
}

func atoi(b []byte) int {
	var n int
	for i := 0; i < len(b); i++ {
		n = n*10 + int(b[i]-'0')
	}
	return n
}

func stringToBytes(s string) []byte {
	return unsafe.Slice(unsafe.StringData(s), len(s))
}
