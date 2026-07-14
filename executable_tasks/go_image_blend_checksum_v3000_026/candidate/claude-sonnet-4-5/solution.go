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
			base := i * 4
			
			// Pixel 0
			a0 := uint32(src[base+3])
			invA0 := 255 - a0
			out[outIdx] = uint8((uint32(src[base])*a0 + uint32(dst[base])*invA0 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+1])*a0 + uint32(dst[base+1])*invA0 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+2])*a0 + uint32(dst[base+2])*invA0 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = 255
			hash ^= 255
			hash *= 1099511628211
			outIdx++
			
			// Pixel 1
			base += 4
			a1 := uint32(src[base+3])
			invA1 := 255 - a1
			out[outIdx] = uint8((uint32(src[base])*a1 + uint32(dst[base])*invA1 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+1])*a1 + uint32(dst[base+1])*invA1 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+2])*a1 + uint32(dst[base+2])*invA1 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = 255
			hash ^= 255
			hash *= 1099511628211
			outIdx++
			
			// Pixel 2
			base += 4
			a2 := uint32(src[base+3])
			invA2 := 255 - a2
			out[outIdx] = uint8((uint32(src[base])*a2 + uint32(dst[base])*invA2 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+1])*a2 + uint32(dst[base+1])*invA2 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+2])*a2 + uint32(dst[base+2])*invA2 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = 255
			hash ^= 255
			hash *= 1099511628211
			outIdx++
			
			// Pixel 3
			base += 4
			a3 := uint32(src[base+3])
			invA3 := 255 - a3
			out[outIdx] = uint8((uint32(src[base])*a3 + uint32(dst[base])*invA3 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+1])*a3 + uint32(dst[base+1])*invA3 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = uint8((uint32(src[base+2])*a3 + uint32(dst[base+2])*invA3 + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			out[outIdx] = 255
			hash ^= 255
			hash *= 1099511628211
			outIdx++
		}
		
		// Handle remaining pixels
		for ; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a
			
			out[outIdx] = uint8((uint32(src[base])*a + uint32(dst[base])*invA + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			
			out[outIdx] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*invA + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			
			out[outIdx] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*invA + 127) / 255)
			hash ^= uint64(out[outIdx])
			hash *= 1099511628211
			outIdx++
			
			out[outIdx] = 255
			hash ^= 255
			hash *= 1099511628211
			outIdx++
		}
	}
	
	return hash
}
