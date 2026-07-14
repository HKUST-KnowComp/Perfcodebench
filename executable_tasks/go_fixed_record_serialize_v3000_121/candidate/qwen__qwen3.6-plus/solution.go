package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	out := make([]byte, n*12)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < n; i++ {
			off := i * 12

			v := ids[i]
			out[off] = byte(v)
			hash ^= uint64(v & 0xFF); hash *= 1099511628211
			out[off+1] = byte(v >> 8)
			hash ^= uint64((v >> 8) & 0xFF); hash *= 1099511628211
			out[off+2] = byte(v >> 16)
			hash ^= uint64((v >> 16) & 0xFF); hash *= 1099511628211
			out[off+3] = byte(v >> 24)
			hash ^= uint64(v >> 24); hash *= 1099511628211

			v = ts[i]
			out[off+4] = byte(v)
			hash ^= uint64(v & 0xFF); hash *= 1099511628211
			out[off+5] = byte(v >> 8)
			hash ^= uint64((v >> 8) & 0xFF); hash *= 1099511628211
			out[off+6] = byte(v >> 16)
			hash ^= uint64((v >> 16) & 0xFF); hash *= 1099511628211
			out[off+7] = byte(v >> 24)
			hash ^= uint64(v >> 24); hash *= 1099511628211

			v = uint32(levels[i])
			out[off+8] = byte(v)
			hash ^= uint64(v & 0xFF); hash *= 1099511628211
			out[off+9] = byte(v >> 8)
			hash ^= uint64((v >> 8) & 0xFF); hash *= 1099511628211

			v = uint32(lens[i])
			out[off+10] = byte(v)
			hash ^= uint64(v & 0xFF); hash *= 1099511628211
			out[off+11] = byte(v >> 8)
			hash ^= uint64((v >> 8) & 0xFF); hash *= 1099511628211
		}
	}
	return hash
}
