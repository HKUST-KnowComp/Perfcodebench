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
		
		// Find field boundaries
		var commas [4]int
		commaIdx := 0
		for i := 0; i < len(data) && commaIdx < 4; i++ {
			if data[i] == ',' {
				commas[commaIdx] = i
				commaIdx++
			}
		}
		
		// Parse field 2 (parts[2])
		start2 := commas[1] + 1
		end2 := commas[2]
		b := 0
		for i := start2; i < end2; i++ {
			b = b*10 + int(data[i]-'0')
		}
		
		// Parse field 3 (parts[3])
		start3 := commas[2] + 1
		end3 := commas[3]
		c := 0
		for i := start3; i < end3; i++ {
			c = c*10 + int(data[i]-'0')
		}
		
		// Check field 4 (parts[4]) for "hot"
		start4 := commas[3] + 1
		hot := start4+2 < len(data) && data[start4] == 'h' && data[start4+1] == 'o' && data[start4+2] == 't'
		
		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
