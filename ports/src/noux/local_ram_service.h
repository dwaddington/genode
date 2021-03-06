/*
 * \brief  RAM service provided to Noux processes
 * \author Josef Soentgen
 * \date   2013-04-16
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _NOUX__LOCAL_RAM_SERVICE_H_
#define _NOUX__LOCAL_RAM_SERVICE_H_

/* Genode includes */
#include <base/service.h>

/* Noux includes */
#include <ram_session_component.h>

namespace Noux {

	class Local_ram_service : public Service
	{
		private:

			Rpc_entrypoint         &_ep;

		public:

			Local_ram_service(Rpc_entrypoint &ep)
			:
				Service(Ram_session::service_name()), _ep(ep)
			{ }

			Genode::Session_capability session(const char *args)
			{
				PDBG("Implement me!");
				return Genode::Session_capability();
			}

			void upgrade(Genode::Session_capability, const char *args)
			{
				PDBG("Implement me!");
			}

			void close(Genode::Session_capability session)
			{
				PDBG("Implement me!");
			}
	};
}

#endif /* _NOUX__LOCAL_RAM_SERVICE_H_ */
