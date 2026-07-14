package main

import (
	"unsafe"
)

const (
	prime64 = 1099511628211
)

//go:nosplit
func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// allocate once, reuse every iteration
	hist := make([]uint64, buckets)
	mem := (*[1 << 30]uint64)(unsafe.Pointer(&hist[0]))[:buckets:buckets]

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// fast clear: 8×uint64 per loop
		for i := 0; i < int(buckets)-7; i += 8 {
			mem[i+0] = 0
			mem[i+1] = 0
			mem[i+2] = 0
			mem[i+3] = 0
			mem[i+4] = 0
			mem[i+5] = 0
			mem[i+6] = 0
			mem[i+7] = 0
		}
		for i := int(buckets) &^ 7; i < int(buckets); i++ {
			mem[i] = 0
		}

		// aggregation
		k := keys
		v := vals
		for len(k) > 0 {
			hist[k[0]] += uint64(v[0])
			k = k[1:]
			v = v[1:]
		}

		// checksum (unrolled)
		h = 1469598103934665603
		for i := 0; i < int(buckets)-3; i += 4 {
			h ^= mem[i+0] + 1
			h *= prime64
			h ^= mem[i+1] + 1
			h *= prime64
			h ^= mem[i+2] + 1
			h *= prime64
			h ^= mem[i+3] + 1
			h *= prime64
		}
		for i := int(buckets) &^ 3; i < int(buckets); i++ {
			h ^= mem[i] + 1
			h *= prime64
		}
	}
	return h
}