package main

import (
	"strconv"
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b, c := parseFields(row)
		v := uint64(b + c + 29)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseFields(row string) (int, int) {
	data := *(*[]byte)(unsafe.Pointer(&struct {
		str *string
		len int
	}{&row, len(row)}))
	
	var commaCount int
	var start int
	var b, c int
	
	for i := 0; i < len(data); i++ {
		if data[i] == ',' {
			commaCount++
			if commaCount == 2 {
				b, _ = strconv.Atoi(row[start:i])
				start = i + 1
			} else if commaCount == 3 {
				c, _ = strconv.Atoi(row[start:i])
				start = i + 1
			} else if commaCount == 4 {
				if i-start == 3 && data[start] == 'h' {
					c += 29
				}
				break
			} else if commaCount == 1 {
				start = i + 1
			}
		}
	}
	
	return b, c
}