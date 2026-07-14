package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	for i := range keys {
		hist[keys[i]] += uint64(vals[i])
	}
	var h uint64
	for iter := 0; iter < iters; iter++ {
		cur := uint64(1469598103934665603)
		for _, v := range hist {
			cur ^= v + 1
			cur *= 1099511628211
		}
		h = cur
	}
	return h
}
