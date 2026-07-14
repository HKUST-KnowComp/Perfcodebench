package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	total := n * 12
	buf := make([]byte, total)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		idx := 0
		for i := 0; i < n; i++ {
			id := ids[i]
			buf[idx] = byte(id)
			hash ^= uint64(buf[idx])
			hash *= 1099511628211
			buf[idx+1] = byte(id >> 8)
			hash ^= uint64(buf[idx+1])
			hash *= 1099511628211
			buf[idx+2] = byte(id >> 16)
			hash ^= uint64(buf[idx+2])
			hash *= 1099511628211
			buf[idx+3] = byte(id >> 24)
			hash ^= uint64(buf[idx+3])
			hash *= 1099511628211

			ts32 := ts[i]
			buf[idx+4] = byte(ts32)
			hash ^= uint64(buf[idx+4])
			hash *= 1099511628211
			buf[idx+5] = byte(ts32 >> 8)
			hash ^= uint64(buf[idx+5])
			hash *= 1099511628211
			buf[idx+6] = byte(ts32 >> 16)
			hash ^= uint64(buf[idx+6])
			hash *= 1099511628211
			buf[idx+7] = byte(ts32 >> 24)
			hash ^= uint64(buf[idx+7])
			hash *= 1099511628211

			lv := levels[i]
			buf[idx+8] = byte(lv)
			hash ^= uint64(buf[idx+8])
			hash *= 1099511628211
			buf[idx+9] = byte(lv >> 8)
			hash ^= uint64(buf[idx+9])
			hash *= 1099511628211

			ln := lens[i]
			buf[idx+10] = byte(ln)
			hash ^= uint64(buf[idx+10])
			hash *= 1099511628211
			buf[idx+11] = byte(ln >> 8)
			hash ^= uint64(buf[idx+11])
			hash *= 1099511628211

			idx += 12
		}
	}
	return hash
}