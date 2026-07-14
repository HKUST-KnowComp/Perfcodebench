package main

import (
	"encoding/json"
	"fmt"
	"strconv"
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
	var state uint64 = 0x9988776655443322
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		u := int((state >> 16) & 0xFFFF)
		state = state*2862933555777941757 + 3037000493
		score := int((state >> 12) & 0x7FF)
		flag := int((state >> 8) & 1)
		rows[i] = fmt.Sprintf("u=%d&score=%d&flag=%d&bucket=b%d", u, score, flag, i%131)
	}
	return rows
}

func parseAt(s string, start int) int {
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
		uPos := strings.Index(row, "u=")
		sPos := strings.Index(row, "&score=")
		fPos := strings.Index(row, "&flag=")
		u := parseAt(row, uPos+2)
		score := parseAt(row, sPos+7)
		flag := parseAt(row, fPos+6)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func main() {
	rows := makeRows(423200)
	expected := expectedChecksum(rows)
	start := time.Now()
	actual := run(rows)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{
		OK:               actual == expected,
		ElapsedNS:        elapsed,
		ExpectedChecksum: expected,
		ActualChecksum:   actual,
	})
	fmt.Println(string(out))
	if actual != expected {
		panic("checksum mismatch")
	}
	_ = strconv.IntSize
}
