package main

import (
	"unsafe"
)

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	hash := uint64(fnvOffset)
	for _, row := range rows {
		b := 0
		c := 0
		hot := false
		s := []byte(row)
		// find 3rd field (index 2)
		comma := 0
		for i := 0; i < len(s); i++ {
			if s[i] == ',' {
				comma++
				if comma == 2 {
					// parse b
					i++
					for ; i < len(s) && s[i] != ','; i++ {
						b = b*10 + int(s[i]-'0')
					}
					// skip comma
					i++
					// parse c
					for ; i < len(s) && s[i] != ','; i++ {
						c = c*10 + int(s[i]-'0')
					}
					// skip comma
					i++
					// check hot flag
					hot = i+2 < len(s) && s[i] == 'h' && s[i+1] == 'o' && s[i+2] == 't'
					break
				}
			}
		}
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}