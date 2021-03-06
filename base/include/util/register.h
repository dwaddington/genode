/*
 * \brief  Generic accessor framework for highly structured memory regions
 * \author Martin stein
 * \date   2011-11-10
 */

/*
 * Copyright (C) 2011-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__UTIL__REGISTER_H_
#define _INCLUDE__UTIL__REGISTER_H_

/* Genode includes */
#include <base/stdint.h>
#include <base/printf.h>

namespace Genode
{
	namespace Trait {

		/**
		 * Properties of integer types with a given bitwidth
		 */
		template <unsigned long _WIDTH> struct Uint_width;

		template <> struct Uint_width<1>
		{
			typedef bool Type;
			enum { WIDTH_LOG2 = 0 };

			/**
			 * Access widths wich are dividers to the compound type width
			 */
			template <unsigned long _DIVISOR_WIDTH> struct Divisor;

			static inline void print_hex(bool const v) {
				printf("%01x", v); }
		};

		template <> struct Uint_width<8> : Uint_width<1>
		{
			typedef uint8_t Type;
			enum { WIDTH_LOG2 = 3 };

			static inline void print_hex(uint8_t const v) {
				printf("%02x", v); }
		};

		template <> struct Uint_width<16> : Uint_width<8>
		{
			typedef uint16_t Type;
			enum { WIDTH_LOG2 = 4 };

			static inline void print_hex(uint16_t const v) {
				printf("%04x", v); }
		};

		template <> struct Uint_width<32> : Uint_width<16>
		{
			typedef uint32_t Type;
			enum { WIDTH_LOG2 = 5 };

			static inline void print_hex (uint32_t const v) {
				printf("%08x", v); }
		};

		template <> struct Uint_width<64> : Uint_width<32>
		{
			typedef uint64_t Type;
			enum { WIDTH_LOG2 = 6 };

			static inline void print_hex(uint64_t const v) {
				printf("%016llx", v); }
		};

		template <> struct Uint_width<1>::Divisor<1>   { enum { WIDTH_LOG2 = 0 }; };
		template <> struct Uint_width<8>::Divisor<2>   { enum { WIDTH_LOG2 = 1 }; };
		template <> struct Uint_width<8>::Divisor<4>   { enum { WIDTH_LOG2 = 2 }; };
		template <> struct Uint_width<8>::Divisor<8>   { enum { WIDTH_LOG2 = 3 }; };
		template <> struct Uint_width<16>::Divisor<16> { enum { WIDTH_LOG2 = 4 }; };
		template <> struct Uint_width<32>::Divisor<32> { enum { WIDTH_LOG2 = 5 }; };
		template <> struct Uint_width<64>::Divisor<64> { enum { WIDTH_LOG2 = 6 }; };

		/**
		 * Reference 'Uint_width' through typenames
		 */
		template <typename _TYPE> struct Uint_type;

		template <> struct Uint_type<bool>     : Uint_width<1> { };
		template <> struct Uint_type<uint8_t>  : Uint_width<8> { };
		template <> struct Uint_type<uint16_t> : Uint_width<16> { };
		template <> struct Uint_type<uint32_t> : Uint_width<32> { };
		template <> struct Uint_type<uint64_t> : Uint_width<64> { };
	}

	/**
	 * An integer like highly structured memory region
	 *
	 * \param  _ACCESS_WIDTH  bit width of the region
	 *
	 * The register can contain multiple bitfields. Bitfields that are
	 * partially exceed the register range are read and written also partially.
	 * Bitfields that are completely out of the register range are read as '0'
	 * and trying to overwrite them has no effect.
	 */
	template <unsigned long _ACCESS_WIDTH>
	struct Register
	{
		enum {
			ACCESS_WIDTH      = _ACCESS_WIDTH,
			ACCESS_WIDTH_LOG2 = Trait::Uint_width<ACCESS_WIDTH>::WIDTH_LOG2,
		};

		typedef typename Trait::Uint_width<ACCESS_WIDTH>::Type access_t;

		/**
		 * A bitregion within a register
		 *
		 * \param  _SHIFT  bit shift of first bit within the compound register
		 * \param  _WIDTH  bit width of the region
		 *
		 * Bitfields are read and written according to their range,
		 * so if we have a 'Bitfield<2,3>' and write '0b11101' to it
		 * only '0b101' (shiftet by 2 bits) is written.
		 */
		template <unsigned long _SHIFT, unsigned long _WIDTH>
		struct Bitfield
		{
			enum {

				/**
				 * Fetch template parameters
				 */
				SHIFT = _SHIFT,
				WIDTH = _WIDTH,
			};

			/**
			 * Get an unshifted mask of this field
			 */
			static access_t mask() { return ((access_t)1 << WIDTH) - 1; }

			/**
			 * Get a mask of this field shifted by its shift in the register
			 */
			static access_t reg_mask() { return mask() << SHIFT; }

			/**
			 * Get the bitwise negation of 'reg_mask'
			 */
			static access_t clear_mask() { return ~reg_mask(); }

			/**
			 * Back reference to containing register
			 */
			typedef Register<ACCESS_WIDTH> Compound_reg;

			/**
			 * Get register with this bitfield set to 'value' and rest left 0
			 *
			 * Useful to combine successive access to multiple
			 * bitfields into one operation.
			 */
			static inline access_t bits(access_t const value) {
				return (value & mask()) << SHIFT; }

			/**
			 * Get a register value 'reg' masked according to this bitfield
			 *
			 * E.g. '0x1234' masked according to a
			 * 'Register<16>::Bitfield<5,7>' returns '0x0220'.
			 */
			static inline access_t masked(access_t const reg)
			{ return reg & reg_mask(); }

			/**
			 * Get value of this bitfield from 'reg'
			 */
			static inline access_t get(access_t const reg)
			{ return (reg >> SHIFT) & mask(); }

			/**
			 * Get registervalue 'reg' with this bitfield set to zero
			 */
			static inline void clear(access_t & reg) { reg &= clear_mask(); }

			/**
			 * Get registervalue 'reg' with this bitfield set to 'value'
			 */
			static inline void set(access_t & reg, access_t const value = ~0)
			{
				clear(reg);
				reg |= (value & mask()) << SHIFT;
			};
		};
	};
}

#endif /* _INCLUDE__UTIL__REGISTER_H_ */

