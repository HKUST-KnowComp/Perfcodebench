package main

func run(rows []string) uint64 {
	const basis = 1469598103934665603
	const prime = 1099511628211
	hash := basis
	for _, row := range rows {
		data := []byte(row)
		var u, score, flag int
		pos := 2 // skip "u="
		for i := pos; i < len(data); i++ {
			if data[i] == '&' {
				for j := pos; j < i; j++ {
					u = u*10 + int(data[j]-'0')
				}
				pos = i + 7 // skip "&score="
				break
			}
		}
		for i := pos; i < len(data); i++ {
			if data[i] == '&' {
				for j := pos; j < i; j++ {
					score = score*10 + int(data[j]-'0')
				}
				pos = i + 6 // skip "&flag="
				break
			}
		}
		for i := pos; i < len(data); i++ {
			flag = flag*10 + int(data[i]-'0')
		}
		v := uint64((u&2047)+score) + uint64(flag)*19
		hash ^= (v + 1)
		hash *= prime
	}
	return hash
}