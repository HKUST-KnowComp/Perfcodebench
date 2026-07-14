package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	for iter := 0; iter < iters; iter++ {
		for i := range hist {
			hist[i] = 0
		}
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		h = 1469598103934665603
		for i := range hist {
			h ^= hist[i] + 1
			h *= 1099511628211
		}
	}
	return h
}
