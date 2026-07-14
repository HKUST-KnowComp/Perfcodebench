package main

import "unsafe"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		b := *(*[]byte)(unsafe.Pointer(&struct {
			str string
			cap int
		}{row, len(row)}))
		n := len(b)
		user := 0
		score := 0
		flag := false
		i := 0
		for i < n {
			if b[i] == '"' {
				i++
				if i+6 <= n && b[i] == 'u' && b[i+1] == 's' && b[i+2] == 'e' && b[i+3] == 'r' && b[i+4] == '"' && b[i+5] == ':' {
					i += 6
					for i < n && (b[i] < '0' || b[i] > '9') {
						i++
					}
					for i < n && b[i] >= '0' && b[i] <= '9' {
						user = user*10 + int(b[i]-'0')
						i++
					}
					continue
				}
				if i+7 <= n && b[i] == 's' && b[i+1] == 'c' && b[i+2] == 'o' && b[i+3] == 'r' && b[i+4] == 'e' && b[i+5] == '"' && b[i+6] == ':' {
					i += 7
					for i < n && (b[i] < '0' || b[i] > '9') {
						i++
					}
					for i < n && b[i] >= '0' && b[i] <= '9' {
						score = score*10 + int(b[i]-'0')
						i++
					}
					continue
				}
				if i+11 <= n && b[i] == 'f' && b[i+1] == 'l' && b[i+2] == 'a' && b[i+3] == 'g' && b[i+4] == '"' && b[i+5] == ':' && b[i+6] == 't' && b[i+7] == 'r' && b[i+8] == 'u' && b[i+9] == 'e' {
					flag = true
					i += 10
					continue
				}
			}
			i++
		}
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
