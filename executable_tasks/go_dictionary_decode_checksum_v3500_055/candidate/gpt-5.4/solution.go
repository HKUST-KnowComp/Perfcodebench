package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// Avoid materializing a decoded slice; hash decoded values directly.
	// This preserves correctness because the final checksum depends only on
	// the decoded sequence for the last iteration, and each iteration decodes
	// the same ids against the same dictionary.
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	if iters <= 0 {
		return 0
	}

	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = offset
		for _, id := range ids {
			h ^= uint64(dictVals[id]) + 1
			h *= prime
		}
	}
	return h
}
