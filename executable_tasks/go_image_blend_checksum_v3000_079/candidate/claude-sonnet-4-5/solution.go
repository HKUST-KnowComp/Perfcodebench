package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		outIdx := 0
		
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			b0 := uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			hash ^= uint64(b0)
			hash *= 1099511628211
			out[outIdx] = b0
			outIdx++
			
			b1 := uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			hash ^= uint64(b1)
			hash *= 1099511628211
			out[outIdx] = b1
			outIdx++
			
			b2 := uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			hash ^= uint64(b2)
			hash *= 1099511628211
			out[outIdx] = b2
			outIdx++
			
			hash ^= 255
			hash *= 1099511628211
			out[outIdx] = 255
			outIdx++
		}
	}
	
	return hash
}