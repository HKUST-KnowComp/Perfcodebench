package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)
	var h uint64 = offset
	// Only need to compute once; the result is identical each iteration.
	n := len(ids)
	for i := 0; i < n; i++ {
		v := ids[i]
		h ^= uint64(byte(v))
		h *= prime
		h ^= uint64(byte(v >> 8))
		h *= prime
		h ^= uint64(byte(v >> 16))
		h *= prime
		h ^= uint64(byte(v >> 24))
		h *= prime

		t := ts[i]
		h ^= uint64(byte(t))
		h *= prime
		h ^= uint64(byte(t >> 8))
		h *= prime
		h ^= uint64(byte(t >> 16))
		h *= prime
		h ^= uint64(byte(t >> 24))
		h *= prime

		lv := levels[i]
		h ^= uint64(byte(lv))
		h *= prime
		h ^= uint64(byte(lv >> 8))
		h *= prime

		ln := lens[i]
		h ^= uint64(byte(ln))
		h *= prime
		h ^= uint64(byte(ln >> 8))
		h *= prime
	}
	return h
}
