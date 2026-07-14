package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Convert string to byte slice without allocation
		data := *(*[]byte)(unsafe.Pointer(&row))
		
		// Find commas manually to avoid strings.Split allocation
		commas := [4]int{}
		count := 0
		for i := 0; i < len(data) && count < 4; i++ {
			if data[i] == ',' {
				commas[count] = i
				count++
			}
		}

		// Parse integers manually
		parse := func(start, end int) int {
			res := 0
			for i := start; i < end; i++ {
				res = res*10 + int(data[i]-'0')
			}
			return res
		}

		b := parse(commas[1]+1, commas[2])
		c := parse(commas[2]+1, commas[3])
		
		// Check for 'hot' at the end
		hot := false
		if len(data)-commas[3] >= 4 && data[commas[3]+1] == 'h' {
			hot = true
		}

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}