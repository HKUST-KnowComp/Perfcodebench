package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := uint32(src[base+3])
			na := 255 - a
			out[base] = uint8((uint32(src[base])*a + uint32(dst[base])*na + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*na + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*na + 127) / 255)
			out[base+3] = 255
		}

		// FNV-1a hash
		hash = 1469598103934665603
		n := outLen
		i := 0
		// Unroll by 8
		for ; i+7 < n; i += 8 {
			hash ^= uint64(out[i])
			hash *= 1099511628211
			hash ^= uint64(out[i+1])
			hash *= 1099511628211
			hash ^= uint64(out[i+2])
			hash *= 1099511628211
			hash ^= uint64(out[i+3])
			hash *= 1099511628211
			hash ^= uint64(out[i+4])
			hash *= 1099511628211
			hash ^= uint64(out[i+5])
			hash *= 1099511628211
			hash ^= uint64(out[i+6])
			hash *= 1099511628211
			hash ^= uint64(out[i+7])
			hash *= 1099511628211
		}
		for ; i < n; i++ {
			hash ^= uint64(out[i])
			hash *= 1099511628211
		}
	}
	return hash
}