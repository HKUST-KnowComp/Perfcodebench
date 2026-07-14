package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	b := int(buckets)
	if iters <= 0 {
		return h
	}

	// Allocate once and reuse across iterations
	hist := make([]uint64, b)
	stamps := make([]uint32, b)
	var cur uint32 = 1

	const fnvInit uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	kl := len(keys)
	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram using generation stamps
		for i := 0; i < kl; i++ {
			k := int(keys[i])
			if stamps[k] != cur {
				stamps[k] = cur
				hist[k] = uint64(vals[i])
			} else {
				hist[k] += uint64(vals[i])
			}
		}

		// Compute checksum over all buckets; treat unstamped entries as zero
		h = fnvInit
		for i := 0; i < b; i++ {
			var v uint64
			if stamps[i] == cur {
				v = hist[i]
			} else {
				v = 0
			}
			h ^= v + 1
			h *= fnvPrime
		}

		// Advance generation; very rarely reset to avoid wrap issues
		cur++
		if cur == 0 {
			for i := 0; i < b; i++ {
				stamps[i] = 0
			}
			cur = 1
		}
	}
	return h
}
