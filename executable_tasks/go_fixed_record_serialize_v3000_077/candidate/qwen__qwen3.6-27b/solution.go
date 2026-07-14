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
		base := 0
		for i := 0; i < n; i++ {
			id := ids[i]
			out[base] = byte(id)
			out[base+1] = byte(id >> 8)
			out[base+2] = byte(id >> 16)
			out[base+3] = byte(id >> 24)
			
			t := ts[i]
			out[base+4] = byte(t)
			out[base+5] = byte(t >> 8)
			out[base+6] = byte(t >> 16)
			out[base+7] = byte(t >> 24)
			
			lv := levels[i]
			out[base+8] = byte(lv)
			out[base+9] = byte(lv >> 8)
			
			ln := lens[i]
			out[base+10] = byte(ln)
			out[base+11] = byte(ln >> 8)
			
			base += 12
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}