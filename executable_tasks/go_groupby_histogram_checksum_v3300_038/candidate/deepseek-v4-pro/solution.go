package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	for i, k := range keys {
		hist[k] += uint64(vals[i])
	}
	var h uint64 = 1469598103934665603
	for _, v := range hist {
		h ^= v + 1
		h *= 1099511628211
	}
	return h
}