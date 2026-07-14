package main

import (
	"encoding/json"
	"fmt"
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
	var state uint64 = 0x1234432155667788
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		a := byte('A' + byte((state>>60)%26))
		b := byte('A' + byte((state>>56)%26))
		x := int((state >> 16) & 9999)
		if (i % 7) == 0 {
			rows[i] = fmt.Sprintf("%c%c%04d-%08X", a, b, x, uint32(state))
		} else {
			rows[i] = fmt.Sprintf("bad:%08X", uint32(state))
		}
	}
	return rows
}

func validHex(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}

func matches(s string) bool {
	if len(s) != 15 {
		return false
	}
	if s[0] < 'A' || s[0] > 'Z' || s[1] < 'A' || s[1] > 'Z' {
		return false
	}
	for i := 2; i < 6; i++ {
		if s[i] < '0' || s[i] > '9' {
			return false
		}
	}
	if s[6] != '-' {
		return false
	}
	for i := 7; i < 15; i++ {
		if !validHex(s[i]) {
			return false
		}
	}
	return true
}

func expectedChecksum(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if matches(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func main() {
	rows := makeRows(101600)
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
