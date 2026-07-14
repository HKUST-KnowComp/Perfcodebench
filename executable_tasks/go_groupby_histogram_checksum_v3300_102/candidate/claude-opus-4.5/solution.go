package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	n := int(buckets)
	hist := make([]uint64, n)
	for iter := 0; iter < iters; iter++ {
		// Fast zero using compiler-recognized pattern
		for i := range hist {
			hist[i] = 0
		}
		// Aggregate
		for i, k := range keys {
			hist[k] += uint64(vals[i])
		}
		// FNV-1a style checksum
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}
