package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	b := int(buckets)
	if b == 0 {
		var h uint64 = 1469598103934665603
		for iter := 0; iter < iters; iter++ {
			h = 1469598103934665603
		}
		return h
	}

	hist := make([]uint64, b)
	used := make([]uint32, 0, b)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		for i, k := range keys {
			idx := int(k)
			if hist[idx] == 0 {
				used = append(used, k)
			}
			hist[idx] += uint64(vals[i])
		}

		h = 1469598103934665603
		for i := 0; i < b; i++ {
			h ^= hist[i] + 1
			h *= 1099511628211
		}

		for _, k := range used {
			hist[int(k)] = 0
		}
		used = used[:0]
	}

	return h
}
