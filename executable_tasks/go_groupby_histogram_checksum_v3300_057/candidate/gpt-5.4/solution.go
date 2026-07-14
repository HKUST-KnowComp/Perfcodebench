package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	b := int(buckets)
	hist := make([]uint64, b)
	used := make([]uint32, 0, b)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		used = used[:0]

		for i, k := range keys {
			idx := int(k)
			if hist[idx] == 0 {
				used = append(used, k)
			}
			hist[idx] += uint64(vals[i])
		}

		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}

		for _, k := range used {
			hist[int(k)] = 0
		}
	}
	return h
}
