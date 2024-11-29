#ifndef _SPECTRE_IO_DMA_H
#define _SPECTRE_IO_DMA_H

#include <asm/io.h>

#define BCM2836_IO_MEM_START	0x3F000000

// -----------------------------------------------------------------------------
// 8-bit IO
// -----------------------------------------------------------------------------

/**
 * Reads a 8-bit IO register.
 *
 * @param addr The IO register address.
 *
 * @returns The value.
 *
 */
static inline u8 dma_read8( void __iomem* addr ) {
	return ioread8( addr );
}

/**
 * Writes a 8-bit IO register.
 *
 * @param addr The IO register address.
 * @param value The value.
 *
 */
static inline void dma_write8( void __iomem* addr, u8 value ) {
	iowrite8( value, addr );
}

/**
 * Reads a series of bits in an 8-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to get.
 *
 * @returns The flag values.
 *
 */
static inline u8 dma_get_flags8( void __iomem* addr, u8 bit, u8 mask ) {
	return ioread8( addr ) & mask;
}

/**
 * Clears flags in an 8-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to clear.
 *
 */
static inline void dma_clr_flags8( void __iomem* addr, u8 flags ) {
	iowrite8( ioread8( addr ) & ~flags, addr );
}

/**
 * Sets flags in an 8-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to set.
 *
 */
static inline void dma_set_flags8( void __iomem* addr, u8 flags ) {
	iowrite8( ioread8( addr ) | flags, addr );
}

// -----------------------------------------------------------------------------
// 16-bit IO
// -----------------------------------------------------------------------------

/**
 * Reads a 16-bit IO register.
 *
 * @param addr The IO register address.
 *
 * @returns The value.
 *
 */
static inline u16 dma_read16( void __iomem* addr ) {
	return ioread16( addr );
}

/**
 * Writes a 16-bit IO register.
 *
 * @param addr The IO register address.
 * @param value The value.
 *
 */
static inline void dma_write16( void __iomem* addr, u16 value ) {
	iowrite16( value, addr );
}

/**
 * Gets flags in a 16-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to get.
 *
 * @returns The flag values.
 *
 */
static inline u16 dma_get_flags16( void __iomem* addr, u16 flags ) {
	return ioread16( addr ) & flags;
}

/**
 * Clears flags in a 16-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to clear.
 *
 */
static inline void dma_clr_flags16( void __iomem* addr, u16 flags ) {
	iowrite16( ioread16( addr ) & ~flags, addr );
}

/**
 * Sets flags in a 16-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to set.
 *
 */
static inline void dma_set_flags16( void __iomem* addr, u16 flags ) {
	iowrite16( ioread16( addr ) | flags, addr );
}

// -----------------------------------------------------------------------------
// 32-bit IO
// -----------------------------------------------------------------------------

/**
 * Reads a 32-bit IO register.
 *
 * @param addr The IO register address.
 *
 * @returns The value.
 *
 */
static inline u32 dma_read32( void __iomem* addr ) {
	return ioread32( addr );
}

/**
 * Writes a 32-bit IO register.
 *
 * @param addr The IO register address.
 * @param value The value.
 *
 */
static inline void dma_write32( void __iomem* addr, u32 value ) {
	iowrite32( value, addr );
}

/**
 * Gets flags in a 32-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to get.
 *
 * @returns The flag values.
 *
 */
static inline u32 dma_get_flags32( void __iomem* addr, u32 flags ) {
	return ioread32( addr ) & flags;
}

/**
 * Clears flags in a 32-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to clear.
 *
 */
static inline void dma_clr_flags32( void __iomem* addr, u32 flags ) {
	iowrite32( ioread32( addr ) & ~flags, addr );
}

/**
 * Sets flags in a 32-bit IO register.
 *
 * @param addr The IO register address.
 * @param flags The flags to set.
 *
 */
static inline void dma_set_flags32( void __iomem* addr, u32 flags ) {
	iowrite32( ioread32( addr ) | flags, addr );
}

#endif // _SPECTRE_IO_DMA_H

