package main

import "strings"

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		firstComma := strings.Index(row, ",")
		secondComma := strings.Index(row[firstComma+1:], ",") + firstComma + 1
		thirdComma := strings.Index(row[secondComma+1:], ",") + secondComma + 1
		fourthComma := strings.Index(row[thirdComma+1:], ",") + thirdComma + 1

		bStr := row[secondComma+1:thirdComma]
		cStr := row[thirdComma+1:fourthComma]
		hotStr := row[fourthComma+1:]

		var b, c int
		for i := range bStr {
			b = b*10 + int(bStr[i]-'0')
		}
		for i := range cStr {
			c = c*10 + int(cStr[i]-'0')
		}

		hot := hotStr == "hot"
		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}