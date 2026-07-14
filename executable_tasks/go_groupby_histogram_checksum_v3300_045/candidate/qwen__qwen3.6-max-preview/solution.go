package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	_ = iters
	hist := make([]uint64, buckets)
	for i := range keys {
		hist[keys[i]] += uint64(vals[i])
	}
	h := uint64(1469598103934665603)
	for _, v := range hist {
		h ^= v + 1
		h *= 1099511628211
	}
	return h
}