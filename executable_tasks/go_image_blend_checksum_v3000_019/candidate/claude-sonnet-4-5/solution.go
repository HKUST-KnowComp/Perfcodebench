package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		outIdx := 0
		
		i := 0
		for ; i+3 < pixels; i += 4 {
			for p := 0; p < 4; p++ {
				base := (i+p) * 4
				a := uint32(src[base+3])
				invA := 255 - a
				
				b0 := uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
				b1 := uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
				b2 := uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
				
				out[outIdx] = b0
				out[outIdx+1] = b1
				out[outIdx+2] = b2
				out[outIdx+3] = 255
				
				hash ^= uint64(b0)
				hash *= 1099511628211
				hash ^= uint64(b1)
				hash *= 1099511628211
				hash ^= uint64(b2)
				hash *= 1099511628211
				hash ^= 255
				hash *= 1099511628211
				
				outIdx += 4
			}
		}
		
		for ; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			b0 := uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			b1 := uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			b2 := uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			
			out[outIdx] = b0
			out[outIdx+1] = b1
			out[outIdx+2] = b2
			out[outIdx+3] = 255
			
			hash ^= uint64(b0)
			hash *= 1099511628211
			hash ^= uint64(b1)
			hash *= 1099511628211
			hash ^= uint64(b2)
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
			
			outIdx += 4
		}
	}
	
	return hash
}
