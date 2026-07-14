package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a

			// Optimized blend: (s*a + d*(255-a) + 127) / 255
			// Using (x + 127) / 255 is equivalent to ((x + 128) * 257) >> 16
			out[base] = uint8(((uint32(src[base])*a + uint32(dst[base])*invA + 127) * 257) >> 16)
			out[base+1] = uint8(((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) * 257) >> 16)
			out[base+2] = uint8(((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) * 257) >> 16)
			out[base+3] = 255
		}

		// Checksum calculation
		hash = 1469598103934665603
		for i := 0; i < len(out); i++ {
			hash ^= uint64(out[i])
			hash *= 1099511628211
		}
	}
	return hash
}