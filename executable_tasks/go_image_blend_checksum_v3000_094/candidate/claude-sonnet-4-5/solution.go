package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		i := 0
		outIdx := 0
		
		// Unroll by 4 pixels
		for ; i < pixels-3; i += 4 {
			// Pixel 0
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			out[outIdx] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[outIdx+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[outIdx+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[outIdx+3] = 255
			
			// Pixel 1
			base += 4
			a = uint32(src[base+3])
			invA = 255 - a
			out[outIdx+4] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[outIdx+5] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[outIdx+6] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[outIdx+7] = 255
			
			// Pixel 2
			base += 4
			a = uint32(src[base+3])
			invA = 255 - a
			out[outIdx+8] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[outIdx+9] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[outIdx+10] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[outIdx+11] = 255
			
			// Pixel 3
			base += 4
			a = uint32(src[base+3])
			invA = 255 - a
			out[outIdx+12] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[outIdx+13] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[outIdx+14] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[outIdx+15] = 255
			
			outIdx += 16
		}
		
		// Handle remaining pixels
		for ; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			out[outIdx] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			out[outIdx+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			out[outIdx+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			out[outIdx+3] = 255
			outIdx += 4
		}
		
		// Inline checksum
		hash = 1469598103934665603
		for _, b := range out {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	
	return hash
}
