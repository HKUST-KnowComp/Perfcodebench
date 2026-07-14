package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b, c, hot := parseRow(row)
		v := uint64(b + c + 11)
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
	
	// Parse field 4 (hot check)
	hot := i+2 < n && bytes[i] == 'h' && bytes[i+1] == 'o' && bytes[i+2] == 't'
	
	return b, c, hot
}

func atoi(b []byte) int {
	n := 0
	neg := false
	if len(b) > 0 && b[0] == '-' {
		neg = true
		b = b[1:]
	}
	for _, c := range b {
		n = n*10 + int(c-'0')
	}
	if neg {
		return -n
	}
	return n
}

func stringToBytes(s string) []byte {
	return unsafe.Slice(unsafe.StringData(s), len(s))
}
