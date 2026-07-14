package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	n := len(keys)
	histLen := len(hist)
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		h = 1469598103934665603
		for i := 0; i < histLen; i++ {
			v := hist[i]
			h ^= v + 1
			h *= 1099511628211
			hist[i] = 0
		}
	}
	return h
}