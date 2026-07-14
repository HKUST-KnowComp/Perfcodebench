package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4 (0-indexed)
		// We need parts[2], parts[3], parts[4]
		// So we need to find commas 0,1,2,3,4
		var c0, c1, c2, c3, c4 int = -1, -1, -1, -1, -1
		count := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				switch count {
				case 0:
					c0 = i
				case 1:
					c1 = i
				case 2:
					c2 = i
				case 3:
					c3 = i
				case 4:
					c4 = i
					goto done
				}
				count++
			}
		}
		c4 = len(row)
	done:
		_ = c0
		// parts[2] is between c1+1 and c2
		// parts[3] is between c2+1 and c3
		// parts[4] is between c3+1 and c4
		b := atoi(row[c1+1 : c2])
		c := atoi(row[c2+1 : c3])
		hot := (c4-c3-1) == 3 && row[c3+1] == 'h' && row[c3+2] == 'o' && row[c3+3] == 't'
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func atoi(s string) int {
	if len(s) == 0 {
		return 0
	}
	neg := false
	i := 0
	if s[0] == '-' {
		neg = true
		i = 1
	} else if s[0] == '+' {
		i = 1
	}
	n := 0
	for ; i < len(s); i++ {
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}