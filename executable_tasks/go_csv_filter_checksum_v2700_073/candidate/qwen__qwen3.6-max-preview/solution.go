package main

import "strconv"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int
		var hot bool
		commaCount := 0
		fieldStart := 0

	Loop:
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				switch commaCount {
				case 2:
					b, _ = strconv.Atoi(row[fieldStart:i])
				case 3:
					c, _ = strconv.Atoi(row[fieldStart:i])
				case 4:
					s := row[fieldStart:i]
					hot = len(s) >= 3 && s[0] == 'h' && s[1] == 'o' && s[2] == 't'
					break Loop
				}
				commaCount++
				fieldStart = i + 1
			}
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
