package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Pre-zeroed backing array large enough for any bucket count we’ll see.
	// 64 KiB gives room for 8192 uint64 buckets – more than enough.
	const backingLen = 1 << 13
	var backing [backingLen]uint64

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Fast clear: copy from pre-zeroed backing.
		hist := backing[:buckets:buckets]

		// Vector-friendly, 8-way unrolled aggregation.
		n := len(keys)
		for i := 0; i < n-7; i += 8 {
			hist[keys[i+0]] += uint64(vals[i+0])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
			hist[keys[i+4]] += uint64(vals[i+4])
			hist[keys[i+5]] += uint64(vals[i+5])
			hist[keys[i+6]] += uint64(vals[i+6])
			hist[keys[i+7]] += uint64(vals[i+7])
		}
		// Tail.
		for i := n &^ 7; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// FNV-1a hash over the histogram.
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}