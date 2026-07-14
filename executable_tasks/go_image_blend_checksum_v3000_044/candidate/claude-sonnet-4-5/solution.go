package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		// Blend loop with direct indexing
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			// Inline blend computation
			out[base] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[base+3] = 255
		}
		
		// Optimized checksum with loop unrolling
		hash = 1469598103934665603
		i := 0
		for ; i+7 < outLen; i += 8 {
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
		for ; i < outLen; i++ {
			hash ^= uint64(out[i])
			hash *= 1099511628211
		}
	}
	return hash
}