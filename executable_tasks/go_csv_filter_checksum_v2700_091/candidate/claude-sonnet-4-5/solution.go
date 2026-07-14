package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual field extraction without Split allocation
		var fieldStart, fieldIdx int
		var b, c int
		var hot bool
		
		for i := 0; i < len(row); i++ {
			if row[i] == ',' || i == len(row)-1 {
				fieldEnd := i
				if i == len(row)-1 {
					fieldEnd = i + 1
				}
				
				switch fieldIdx {
				case 2:
					b, _ = strconv.Atoi(row[fieldStart:fieldEnd])
				case 3:
					c, _ = strconv.Atoi(row[fieldStart:fieldEnd])
				case 4:
					hot = fieldEnd-fieldStart == 3 && row[fieldStart] == 'h'
					if fieldIdx == 4 {
						goto compute
					}
				}
				
				fieldIdx++
				fieldStart = i + 1
			}
		}
		
	compute:
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
