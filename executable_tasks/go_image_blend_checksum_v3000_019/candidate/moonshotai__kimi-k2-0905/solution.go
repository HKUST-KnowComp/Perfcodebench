package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)

	// constants
	const (
		c127    = uint64(0x007F007F007F007F)
		c255    = uint64(0x00FF00FF00FF00FF)
		c255inv = uint64(0x00FF00FF00FF00FF) // 255-a is handled via (255-a) below
		maskLo  = uint64(0x00FF00FF00FF00FF)
		maskHi  = uint64(0xFF00FF00FF00FF00)
	)

	var hash uint64 = 1469598103934665603

	for iter := 0; iter < iters; iter++ {
		// reset output slice header cheaply
		outPtr := out

		for i := 0; i < pixels; {
			// process 4 RGBA pixels (16 bytes) per iteration
			// load src & dst 8 bytes at a time (two pixels each)

			// pixel 0 & 1
			s0 := uint64(src[i*4+0]) | uint64(src[i*4+1])<<8 | uint64(src[i*4+2])<<16 | uint64(src[i*4+3])<<24 |
				uint64(src[i*4+4])<<32 | uint64(src[i*4+5])<<40 | uint64(src[i*4+6])<<48 | uint64(src[i*4+7])<<56
			d0 := uint64(dst[i*4+0]) | uint64(dst[i*4+1])<<8 | uint64(dst[i*4+2])<<16 | uint64(dst[i*4+3])<<24 |
				uint64(dst[i*4+4])<<32 | uint64(dst[i*4+5])<<40 | uint64(dst[i*4+6])<<48 | uint64(dst[i*4+7])<<56

			// pixel 2 & 3
			s1 := uint64(src[i*4+8]) | uint64(src[i*4+9])<<8 | uint64(src[i*4+10])<<16 | uint64(src[i*4+11])<<24 |
				uint64(src[i*4+12])<<32 | uint64(src[i*4+13])<<40 | uint64(src[i*4+14])<<48 | uint64(src[i*4+15])<<56
			d1 := uint64(dst[i*4+8]) | uint64(dst[i*4+9])<<8 | uint64(dst[i*4+10])<<16 | uint64(dst[i*4+11])<<24 |
				uint64(dst[i*4+12])<<32 | uint64(dst[i*4+13])<<40 | uint64(dst[i*4+14])<<48 | uint64(dst[i*4+15])<<56

			// extract alpha channels (every 4th byte)
			a0 := (s0 >> 24) & 0xFF
			a1 := (s0 >> 56) & 0xFF
			a2 := (s1 >> 24) & 0xFF
			a3 := (s1 >> 56) & 0xFF

			// replicate alpha for blending
			aMask0 := a0 | a0<<8 | a0<<16 | a0<<24
			aMask1 := a1 | a1<<8 | a1<<16 | a1<<24
			aMask2 := a2 | a2<<8 | a2<<16 | a2<<24
			aMask3 := a3 | a3<<8 | a3<<16 | a3<<24

			// blend for pixel 0
			sLo0 := s0 & maskLo
			sHi0 := (s0 >> 8) & maskLo
			dLo0 := d0 & maskLo
			dHi0 := (d0 >> 8) & maskLo

			resLo0 := (sLo0*aMask0 + dLo0*(255-aMask0) + c127) / 255
			resHi0 := (sHi0*aMask0 + dHi0*(255-aMask0) + c127) / 255
			out0 := (resLo0 & maskLo) | ((resHi0 & maskLo) << 8)

			// blend for pixel 1
			sLo1 := (s0 >> 32) & maskLo
			sHi1 := (s0 >> 40) & maskLo
			dLo1 := (d0 >> 32) & maskLo
			dHi1 := (d0 >> 40) & maskLo

			resLo1 := (sLo1*aMask1 + dLo1*(255-aMask1) + c127) / 255
			resHi1 := (sHi1*aMask1 + dHi1*(255-aMask1) + c127) / 255
			out1 := (resLo1 & maskLo) | ((resHi1 & maskLo) << 8)

			// blend for pixel 2
			sLo2 := s1 & maskLo
			sHi2 := (s1 >> 8) & maskLo
			dLo2 := d1 & maskLo
			dHi2 := (d1 >> 8) & maskLo

			resLo2 := (sLo2*aMask2 + dLo2*(255-aMask2) + c127) / 255
			resHi2 := (sHi2*aMask2 + dHi2*(255-aMask2) + c127) / 255
			out2 := (resLo2 & maskLo) | ((resHi2 & maskLo) << 8)

			// blend for pixel 3
			sLo3 := (s1 >> 32) & maskLo
			sHi3 := (s1 >> 40) & maskLo
			dLo3 := (d1 >> 32) & maskLo
			dHi3 := (d1 >> 40) & maskLo

			resLo3 := (sLo3*aMask3 + dLo3*(255-aMask3) + c127) / 255
			resHi3 := (sHi3*aMask3 + dHi3*(255-aMask3) + c127) / 255
			out3 := (resLo3 & maskLo) | ((resHi3 & maskLo) << 8)

			// store results
			outPtr[i*4+0] = byte(out0)
			outPtr[i*4+1] = byte(out0 >> 8)
			outPtr[i*4+2] = byte(out0 >> 16)
			outPtr[i*4+3] = 255

			outPtr[i*4+4] = byte(out0 >> 32)
			outPtr[i*4+5] = byte(out0 >> 40)
			outPtr[i*4+6] = byte(out0 >> 48)
			outPtr[i*4+7] = 255

			outPtr[i*4+8] = byte(out1)
			outPtr[i*4+9] = byte(out1 >> 8)
			outPtr[i*4+10] = byte(out1 >> 16)
			outPtr[i*4+11] = 255

			outPtr[i*4+12] = byte(out1 >> 32)
			outPtr[i*4+13] = byte(out1 >> 40)
			outPtr[i*4+14] = byte(out1 >> 48)
			outPtr[i*4+15] = 255

			i += 4
		}

		// Handle remaining pixels
		for i := 0; i < pixels*4; i += 4 {
			if i >= pixels*4 {
				break
			}
			a := src[i+3]
			out[i+0] = uint8((uint32(src[i+0])*uint32(a) + uint32(dst[i+0])*uint32(255-a) + 127) / 255)
			out[i+1] = uint8((uint32(src[i+1])*uint32(a) + uint32(dst[i+1])*uint32(255-a) + 127) / 255)
			out[i+2] = uint8((uint32(src[i+2])*uint32(a) + uint32(dst[i+2])*uint32(255-a) + 127) / 255)
			out[i+3] = 255
		}

		// Compute checksum inline
		for _, b := range out {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}