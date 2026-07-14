package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	n := len(keys)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		for i := range hist {
			hist[i] = 0
		}
		for i := 0; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}