package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		if len(row) == 0 {
			continue
		}
		
		// Convert string to []byte without allocation
		data := *(*[]byte)(unsafe.Pointer(&struct {
			str string
			cap int
		}{row, len(row)}))
		
		// Skip first two fields
		commaCount := 0
		pos := 0
		for pos < len(data) && commaCount < 2 {
			if data[pos] == ',' {
				commaCount++
			}
			pos++
		}
		
		// Parse field 2 (b)
		start := pos
		for pos < len(data) && data[pos] != ',' {
			pos++
		}
		b := 0
		for i := start; i < pos; i++ {
			b = b*10 + int(data[i]-'0')
		}
		pos++ // skip comma
		
		// Parse field 3 (c)
		start = pos
		for pos < len(data) && data[pos] != ',' {
			pos++
		}
		c := 0
		for i := start; i < pos; i++ {
			c = c*10 + int(data[i]-'0')
		}
		pos++ // skip comma
		
		// Check field 4 (hot)
		hot := pos+2 < len(data) && data[pos] == 'h' && data[pos+1] == 'o' && data[pos+2] == 't'
		
		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
