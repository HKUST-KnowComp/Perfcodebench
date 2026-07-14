package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		// Process pixels with inlined blend
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := uint32(src[base+3])
			invA := 255 - a
			
			out[base] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[base+3] = 255
		}

		// FNV-1a hash - process 8 bytes at a time
		hash = 1469598103934665603
		const prime = 1099511628211
		
		i := 0
		for ; i+8 <= outLen; i += 8 {
			hash ^= uint64(out[i])
			hash *= prime
			hash ^= uint64(out[i+1])
			hash *= prime
			hash ^= uint64(out[i+2])
			hash *= prime
			hash ^= uint64(out[i+3])
			hash *= prime
			hash ^= uint64(out[i+4])
			hash *= prime
			hash ^= uint64(out[i+5])
			hash *= prime
			hash ^= uint64(out[i+6])
			hash *= prime
			hash ^= uint64(out[i+7])
			hash *= prime
		}
		for ; i < outLen; i++ {
			hash ^= uint64(out[i])
			hash *= prime
		}
	}
	return hash
}