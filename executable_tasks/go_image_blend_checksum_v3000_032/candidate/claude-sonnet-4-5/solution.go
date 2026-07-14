package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		
		i := 0
		outIdx := 0
		
		// Unroll by 4 pixels
		for ; i < pixels-3; i += 4 {
			// Pixel 0
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			r := uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			g := uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			b := uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			
			out[outIdx] = r
			out[outIdx+1] = g
			out[outIdx+2] = b
			out[outIdx+3] = 255
			
			hash ^= uint64(r)
			hash *= 1099511628211
			hash ^= uint64(g)
			hash *= 1099511628211
			hash ^= uint64(b)
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
			
			// Pixel 1
			base = (i + 1) * 4
			a = uint32(src[base+3])
			invA = 255 - a
			
			r = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			g = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			b = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			
			out[outIdx+4] = r
			out[outIdx+5] = g
			out[outIdx+6] = b
			out[outIdx+7] = 255
			
			hash ^= uint64(r)
			hash *= 1099511628211
			hash ^= uint64(g)
			hash *= 1099511628211
			hash ^= uint64(b)
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
			
			// Pixel 2
			base = (i + 2) * 4
			a = uint32(src[base+3])
			invA = 255 - a
			
			r = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			g = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			b = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			
			out[outIdx+8] = r
			out[outIdx+9] = g
			out[outIdx+10] = b
			out[outIdx+11] = 255
			
			hash ^= uint64(r)
			hash *= 1099511628211
			hash ^= uint64(g)
			hash *= 1099511628211
			hash ^= uint64(b)
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
			
			// Pixel 3
			base = (i + 3) * 4
			a = uint32(src[base+3])
			invA = 255 - a
			
			r = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			g = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			b = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			
			out[outIdx+12] = r
			out[outIdx+13] = g
			out[outIdx+14] = b
			out[outIdx+15] = 255
			
			hash ^= uint64(r)
			hash *= 1099511628211
			hash ^= uint64(g)
			hash *= 1099511628211
			hash ^= uint64(b)
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
			
			outIdx += 16
		}
		
		// Handle remaining pixels
		for ; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			r := uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			g := uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			b := uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			
			out[outIdx] = r
			out[outIdx+1] = g
			out[outIdx+2] = b
			out[outIdx+3] = 255
			
			hash ^= uint64(r)
			hash *= 1099511628211
			hash ^= uint64(g)
			hash *= 1099511628211
			hash ^= uint64(b)
			hash *= 1099511628211
			hash ^= 255
			hash *= 1099511628211
			
			outIdx += 4
		}
	}
	
	return hash
}
