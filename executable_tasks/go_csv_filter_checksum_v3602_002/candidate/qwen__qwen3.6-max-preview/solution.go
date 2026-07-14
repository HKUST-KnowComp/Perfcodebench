package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int
		var hot bool
		var fieldIdx int
		var start int
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				switch fieldIdx {
				case 2:
					s := row[start:i]
					if len(s) > 0 && s[0] == '-' {
						for j := 1; j < len(s); j++ {
							b = b*10 + int(s[j]-'0')
						}
						b = -b
					} else {
						for j := 0; j < len(s); j++ {
							b = b*10 + int(s[j]-'0')
						}
					}
				case 3:
					s := row[start:i]
					if len(s) > 0 && s[0] == '-' {
						for j := 1; j < len(s); j++ {
							c = c*10 + int(s[j]-'0')
						}
						c = -c
					} else {
						for j := 0; j < len(s); j++ {
							c = c*10 + int(s[j]-'0')
						}
					}
				case 4:
					hot = row[start:i] == "hot"
					goto compute
				}
				fieldIdx++
				start = i + 1
			}
		}
		if fieldIdx == 4 {
			hot = row[start:] == "hot"
		}
	compute:
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}