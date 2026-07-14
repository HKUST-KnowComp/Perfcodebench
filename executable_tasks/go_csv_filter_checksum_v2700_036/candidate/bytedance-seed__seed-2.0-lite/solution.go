package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var commaPos [5]int
		count := 0
		// Scan first 5 commas to locate required fields
		for i := 0; i < len(row) && count < 5; i++ {
			if row[i] == ',' {
				commaPos[count] = i
				count++
			}
		}
		// Extract fields identical to original parts[2], parts[3], parts[4]
		bStr := row[commaPos[1]+1 : commaPos[2]]
		cStr := row[commaPos[2]+1 : commaPos[3]]
		hotStr := row[commaPos[3]+1:]
		b, _ := strconv.Atoi(bStr)
		c, _ := strconv.Atoi(cStr)
		hot := hotStr == "hot"
		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}