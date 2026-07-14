package main

func run(ids, dictVals []uint32, iters int) uint64 {
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603

		// Decode and checksum in a single pass to avoid the extra allocation,
		// write traffic, and second traversal of a temporary decoded slice.
		for _, id := range ids {
			h ^= uint64(dictVals[id]) + 1
			h *= 1099511628211
		}
	}
	return h
}
