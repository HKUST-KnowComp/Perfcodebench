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

func blendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func checksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func makeInput(pixels int) ([]byte, []byte) {
	src := make([]byte, pixels*4)
	dst := make([]byte, pixels*4)
	var state uint64 = 0x123456789ABCDEF0
	for i := 0; i < pixels; i++ {
		base := i * 4
		for c := 0; c < 3; c++ {
			state = state*6364136223846793005 + 1442695040888963407
			src[base+c] = byte(state >> 56)
			state = state*2862933555777941757 + 3037000493
			dst[base+c] = byte(state >> 56)
		}
		state = state*6364136223846793005 + 1442695040888963407
		src[base+3] = byte(state >> 56)
		dst[base+3] = 255
	}
	return src, dst
}

func expectedChecksum(src, dst []byte, width, height int) uint64 {
	out := make([]byte, len(src))
	for i := 0; i < width*height; i++ {
		base := i * 4
		a := src[base+3]
		out[base] = blendByte(src[base], dst[base], a)
		out[base+1] = blendByte(src[base+1], dst[base+1], a)
		out[base+2] = blendByte(src[base+2], dst[base+2], a)
		out[base+3] = 255
	}
	return checksumBytes(out)
}

func main() {
	width := 96
	height := 164
	iters := 22
	src, dst := makeInput(width * height)
	expected := expectedChecksum(src, dst, width, height)
	start := time.Now()
	actual := run(src, dst, width, height, iters)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{OK: actual == expected, ElapsedNS: elapsed, ExpectedChecksum: expected, ActualChecksum: actual})
	fmt.Println(string(out))
	if actual != expected {
		panic("checksum mismatch")
	}
}
