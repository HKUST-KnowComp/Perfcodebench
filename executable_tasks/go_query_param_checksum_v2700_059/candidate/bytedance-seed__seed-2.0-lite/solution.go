package main

func parseInt(s string) int {
	n := 0
	for i := range s {
		n = n*10 + int(s[i]-'0')
	}
	return n
}

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var amp1, amp2 int = -1, -1
		for i := range row {
			if row[i] == '&' {
				if amp1 == -1 {
					amp1 = i
				} else {
					amp2 = i
					break
				}
			}
		}
		u := parseInt(row[2:amp1])
		score := parseInt(row[amp1+7:amp2])
		flag := parseInt(row[amp2+6:])
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}