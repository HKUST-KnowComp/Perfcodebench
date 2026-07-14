package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		if iter == iters-1 {
			// Final iteration: fuse checksum calculation
			hash = 1469598103934665603
			for i := 0; i < pixels; i++ {
				base := i * 4
				a := uint32(src[base+3])
				invA := 255 - a
				
				outBase := i * 4
				b0 := uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
				b1 := uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
				b2 := uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
				
				out[outBase] = b0
				out[outBase+1] = b1
				out[outBase+2] = b2
				out[outBase+3] = 255
				
				hash ^= uint64(b0)
				hash *= 1099511628211
				hash ^= uint64(b1)
				hash *= 1099511628211
				hash ^= uint64(b2)
				hash *= 1099511628211
				hash ^= 255
				hash *= 1099511628211
			}
		} else {
			// Non-final iterations: blend only
			for i := 0; i < pixels; i++ {
				base := i * 4
				a := uint32(src[base+3])
				invA := 255 - a
				
				outBase := i * 4
				out[outBase] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
				out[outBase+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
				out[outBase+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
				out[outBase+3] = 255
			}
		}
	}
	
	return hash
}
