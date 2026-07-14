package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		data := *(*[]byte)(unsafe.Pointer(&row))
		
		// Find commas to locate fields 2, 3, and 4
		commas := [4]int{}
		count := 0
		for i := 0; i < len(data) && count < 4; i++ {
			if data[i] == ',' {
				commas[count] = i
				count++
			}
		}

		// Parse field 2 (index 2)
		b := 0
		for i := commas[1] + 1; i < commas[2]; i++ {
			b = b*10 + int(data[i]-'0')
		}

		// Parse field 3 (index 3)
		c := 0
		for i := commas[2] + 1; i < commas[3]; i++ {
			c = c*10 + int(data[i]-'0')
		}

		// Check field 4 (index 4) for "hot"
		hot := false
		if len(data) > commas[3]+3 && data[commas[3]+1] == 'h' && data[commas[3]+2] == 'o' && data[commas[3]+3] == 't' {
			hot = true
		}

		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}