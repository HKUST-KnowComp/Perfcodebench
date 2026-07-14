package main

import (
	"bytes"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		rowBytes := []byte(row)
		pos := 0
		for pos < len(rowBytes) {
			// Find the start of a key
			start := bytes.IndexByte(rowBytes[pos:], '"')
			if start == -1 {
				break
			}
			pos += start

			// Check for "user":
			if len(rowBytes)-pos >= 7 && 
				rowBytes[pos] == '"' && rowBytes[pos+1] == 'u' && rowBytes[pos+2] == 's' && 
				rowBytes[pos+3] == 'e' && rowBytes[pos+4] == 'r' && rowBytes[pos+5] == '"' && 
				bytes.IndexByte(rowBytes[pos+6:], ':') != -1 {
				
				// Move to value
				valPos := pos + 6
				for valPos < len(rowBytes) && (rowBytes[valPos] == ':' || rowBytes[valPos] == ' ') {
					valPos++
				}
				for valPos < len(rowBytes) && rowBytes[valPos] >= '0' && rowBytes[valPos] <= '9' {
					user = user*10 + int(rowBytes[valPos]-'0')
					valPos++
				}
				pos = valPos
			} else if len(rowBytes)-pos >= 7 && 
				rowBytes[pos] == '"' && rowBytes[pos+1] == 's' && rowBytes[pos+2] == 'c' && 
				rowBytes[pos+3] == 'o' && rowBytes[pos+4] == 'r' && rowBytes[pos+5] == 'e' && 
				rowBytes[pos+6] == '"' {
				
				valPos := pos + 7
				for valPos < len(rowBytes) && (rowBytes[valPos] == ':' || rowBytes[valPos] == ' ') {
					valPos++
				}
				for valPos < len(rowBytes) && rowBytes[valPos] >= '0' && rowBytes[valPos] <= '9' {
					score = score*10 + int(rowBytes[valPos]-'0')
					valPos++
				}
				pos = valPos
			} else if len(rowBytes)-pos >= 7 && 
				rowBytes[pos] == '"' && rowBytes[pos+1] == 'f' && rowBytes[pos+2] == 'l' && 
				rowBytes[pos+3] == 'a' && rowBytes[pos+4] == 'g' && rowBytes[pos+5] == '"' {
				
				valPos := pos + 6
				for valPos < len(rowBytes) && (rowBytes[valPos] == ':' || rowBytes[valPos] == ' ') {
					valPos++
				}
				if valPos+3 < len(rowBytes) && 
					rowBytes[valPos] == 't' && rowBytes[valPos+1] == 'r' && 
					rowBytes[valPos+2] == 'u' && rowBytes[valPos+3] == 'e' {
					flag = true
				}
				pos = valPos + 1
			} else {
				pos++
			}
	}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}