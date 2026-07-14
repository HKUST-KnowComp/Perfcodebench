package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	hist := make([]uint64, buckets)
	for i, k := range keys {
		hist[k] += uint64(vals[i])
	}
	var h uint64 = 1469598103934665603
	for i := 0; i < len(hist); i++ {
		v := hist[i]
		h ^= v + 1
		h *= 1099511628211
	}
	return h
}
