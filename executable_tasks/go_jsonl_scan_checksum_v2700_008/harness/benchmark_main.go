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

func makeInput(n int) []string {
	rows := make([]string, n)
	var state uint64 = 0x123456789abcdef0
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		uid := state & 65535
		state = state*2862933555777941757 + 3037000493
		score := state & 1023
		flag := "false"
		if (i % 3) == 0 {
			flag = "true"
		}
		rows[i] = fmt.Sprintf("{\"user\":%d,\"score\":%d,\"flag\":%s,\"tag\":\"item_%d\"}", uid, score, flag, i%39)
	}
	return rows
}

func expectedChecksum(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		userPos := strings.Index(row, "\"user\":")
		scorePos := strings.Index(row, "\"score\":")
		flagPos := strings.Index(row, "\"flag\":true")
		if userPos < 0 || scorePos < 0 {
			continue
		}
		userPos += len("\"user\":")
		scorePos += len("\"score\":")
		user := 0
		for userPos < len(row) && row[userPos] >= '0' && row[userPos] <= '9' {
			user = user*10 + int(row[userPos]-'0')
			userPos++
		}
		score := 0
		for scorePos < len(row) && row[scorePos] >= '0' && row[scorePos] <= '9' {
			score = score*10 + int(row[scorePos]-'0')
			scorePos++
		}
		v := uint64((user & 1023) + score)
		if flagPos >= 0 {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func main() {
	rows := makeInput(33600)
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
}
