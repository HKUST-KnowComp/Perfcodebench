package main

import (
	"encoding/json"
	"fmt"
	"strings"
	"time"
)

type result struct {
	OK               bool   `json:"ok"`
	ElapsedNS        int64  `json:"elapsed_ns"`
	ExpectedChecksum uint64 `json:"expected_checksum"`
	ActualChecksum   uint64 `json:"actual_checksum"`
}

func makeRows(n int) []string {
	rows := make([]string, n)
	var state uint64 = 0x8877665544332211
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		a := int((state >> 16) & 65535)
		b := int((state >> 28) & 2047)
		c := int((state >> 40) & 1023)
		flag := "cold"
		if (i % 5) == 0 {
			flag = "hot"
		}
		rows[i] = fmt.Sprintf("r%d,%d,%d,%d,%s", i%997, a, b, c, flag)
	}
	return rows
}

func parseIntAt(s string, start int) int {
	v := 0
	for start < len(s) {
		c := s[start]
		if c < '0' || c > '9' {
			break
		}
		v = v*10 + int(c-'0')
		start++
	}
	return v
}

func expectedChecksum(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		p1 := strings.IndexByte(row, ',')
		p2 := strings.IndexByte(row[p1+1:], ',') + p1 + 1
		p3 := strings.IndexByte(row[p2+1:], ',') + p2 + 1
		p4 := strings.IndexByte(row[p3+1:], ',') + p3 + 1
		b := parseIntAt(row, p2+1)
		c := parseIntAt(row, p3+1)
		hot := row[p4+1] == 'h'
		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func main() {
	rows := makeRows(220500)
	expected := expectedChecksum(rows)
	start := time.Now()
	actual := run(rows)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{OK: actual == expected, ElapsedNS: elapsed, ExpectedChecksum: expected, ActualChecksum: actual})
	fmt.Println(string(out))
	if actual != expected {
		panic("checksum mismatch")
	}
}
