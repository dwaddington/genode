
/*
 * \brief  Generic base of AHCI driver
 * \author Sebastian Sumpf <Sebastian.Sumpf@genode-labs.com>
 * \author Martin Stein    <Martin.Stein@genode-labs.com>
 * \date   2011-08-10
 */

/*
 * Copyright (C) 2011-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _AHCI_DRIVER_BASE_H_
#define _AHCI_DRIVER_BASE_H_

/* local includes */
#include <ahci_device.h>

/**
 * Implementation of block driver interface
 */
class Ahci_driver_base : public Block::Driver
{
	protected:

		Ahci_device *_device;

		void _sanity_check(size_t block_number, size_t count)
		{
			if (!_device || (block_number + count > block_count()))
				throw Io_error();
		}

		Ahci_driver_base(Ahci_device * const device) : _device(device) { }

	public:

		~Ahci_driver_base()
		{
			if (_device)
				destroy(env()->heap(), _device);
		}

		size_t block_size()  { return Ahci_device::block_size(); }
		size_t block_count() { return _device ? _device->block_count() : 0; }
		bool   dma_enabled() { return true; }

		void read_dma(size_t block_number,
		              size_t block_count,
		              addr_t phys)
		{
			_sanity_check(block_number, block_count);
			_device->read(block_number, block_count, phys);
		}

		void write_dma(size_t  block_number,
		               size_t  block_count,
		               addr_t  phys)
		{
			_sanity_check(block_number, block_count);
			_device->write(block_number, block_count, phys);
		}

		void read(size_t, size_t, char *)
		{
			PERR("%s should not be called", __PRETTY_FUNCTION__);
			throw Io_error();
		}

		void write(size_t, size_t, char const *)
		{
			PERR("%s should not be called", __PRETTY_FUNCTION__);
			throw Io_error();
		}

		Ram_dataspace_capability alloc_dma_buffer(size_t size) {
			return _device->alloc_dma_buffer(size); }
};

#endif /* _AHCI_DRIVER_BASE_H_ */

