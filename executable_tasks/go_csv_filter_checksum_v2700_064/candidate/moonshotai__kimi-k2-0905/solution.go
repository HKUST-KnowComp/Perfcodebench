package main

func run(rows []string) uint64 {
	const (
		offsetB      = 2
		offsetC      = 3
		offsetHot    = 4
		base         = 1469598103934665603
		prime        = 1099511628211
		hotBonus     = 29
		coldAdd      = 25
		hotTrueByte  = 'h'
	)
	hash := uint64(base)
	for _, row := range rows {
		b, c, hot := parseRow(row)
		v := uint64(b + c + coldAdd)
		if hot {
			v += hotBonus
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}

// parseRow extracts the 3rd field (b), 4th field (c) and whether the 5th field is "hot".
// It scans the row once without allocations.
func parseRow(row string) (b, c int, hot bool) {
	comma := 0
	start := 0
	// field 0
	for i := 0; i < len(row); i++ {
		if row[i] == ',' {
			if comma == 0 { // end of field 0
				comma++
				start = i + 1
			} else if comma == 1 { // end of field 1
				comma++
				start = i + 1
			} else if comma == 2 { // end of field 2 -> parse b
				b = atoi(row[start:i])
				comma++
				start = i + 1
			} else if comma == 3 { // end of field 3 -> parse c
				c = atoi(row[start:i])
				comma++
				start = i + 1
			} else if comma == 4 { // end of field 4 -> check hot
				hot = (i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't')
				return
			}
		}
	}
	// handle last field if no trailing comma
	if comma == 4 {
		hot = (len(row)-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't')
	}
	return
}

// atoi parses a non-negative integer from a byte slice without allocations.
func atoi(s string) int {
	v := 0
	for i := 0; i < len(s); i++ {
		v = v*10 + int(s[i]-'0')
	}
	return v
}