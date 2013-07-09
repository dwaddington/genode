#include <base/printf.h>
#include <base/sleep.h>
#include <base/env.h>
#include <dataspace/client.h>
#include <cpu_session/connection.h>
#include <rom_session/connection.h>
#include <rm_session/connection.h>
#include <ram_session/connection.h>

#define ASSERT(X,Y) if(!X) { PERR("Assertion failed:%s",Y); asm("int3"); }

#ifndef NULL
#define NULL (0UL)
#endif

#define KB(X) (X << 10)
#define MB(X) (X << 20)
#define GB(X) (((unsigned long)X) << 30)

#define REDUCE_KB(X) (X >> 10)
#define REDUCE_MB(X) (X >> 20)
#define REDUCE_GB(X) (X >> 30)
#define BREAK     asm("int3")

#define NUM_REGIONS 10
#define TOTAL_MEMORY_TO_USE GB(1) // fails!
//#define TOTAL_MEMORY_TO_USE MB(120) // works!

#define REGION_SIZE (TOTAL_MEMORY_TO_USE/NUM_REGIONS)

using namespace Genode;


Dataspace_capability create_sub_rm(Genode::Ram_dataspace_capability ram_ds, addr_t offset,size_t size)
{
  Rm_connection *sub_rm = NULL;

  try {
    sub_rm = new (Genode::env()->heap()) Rm_connection(0UL,size);
    ASSERT(sub_rm,"invalid sub_rm");
    sub_rm->on_destruction(Rm_connection::KEEP_OPEN);
    
    sub_rm->attach(ram_ds,
                   size,
                   offset,
                   false, // use local addr
                   (addr_t)0); 
    
  }
  catch(Genode::Rm_session::Out_of_metadata) {
    PLOG("sub_rm->attach - out_of_meta_data: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(Genode::Rm_session::Invalid_dataspace) {
    PLOG("sub_rm->attach - invalid dataspace: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(Genode::Rm_session::Invalid_args) {
    PLOG("sub_rm->attach - invalid args: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(Genode::Rm_session::Region_conflict) {
    PLOG("sub_rm->attach - region conflict: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(...) {
    PLOG("sub_rm->attach - failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }

  return sub_rm->dataspace();
}

void * attach_to_sub_rm(Dataspace_capability ds) 
{
  void * p = NULL;
  try {
    p = env()->rm_session()->attach(ds);
    ASSERT(p,"rm_session()->attach(ds) return NULL");
  }
  catch(Genode::Rm_session::Out_of_metadata) {
    PLOG("out_of_meta_data: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(Genode::Rm_session::Invalid_dataspace) {
    PLOG("invalid dataspace: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(Genode::Rm_session::Invalid_args) {
    PLOG("invalid args: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(Genode::Rm_session::Region_conflict) {
    PLOG("region conflict: failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  catch(...) {
    PLOG("failed rm_session()->attach at %s",__FUNCTION__);
    BREAK;
  }
  return p;
}

int main()
{
  const size_t phys_slab_size = GB(1);  
  void * p_table[NUM_REGIONS];
  Dataspace_capability sub_rm_table[NUM_REGIONS];


  PLOG("NUM_REGIONS=%d REGION_SIZE=%lu MB",NUM_REGIONS, REDUCE_MB(REGION_SIZE));

  /* allocate physical memory */
  Genode::Ram_connection ram; 
  env()->parent()->upgrade(ram.cap(), "ram_quota=256K");

  //  ram.on_destruction(Ram_connection::KEEP_OPEN);
  ram.ref_account(env()->ram_session_cap());
  
  env()->ram_session()->transfer_quota(ram.cap(), phys_slab_size + (MB(1)));

  Ram_dataspace_capability ram_ds;
  try {
        ram_ds = ram.alloc(phys_slab_size);
        ASSERT(ram_ds.valid(),"invalid ds");
  }
  catch(...) {
    ASSERT(0,"ram.alloc() failed");
  }
  
  PLOG("Allocated physical memory: %lu MB ... OK", REDUCE_MB(Dataspace_client(ram_ds).size()));

  /* create sub-rm dataspaces */
  addr_t curr_offset = 0;
  for(unsigned i=0;i<NUM_REGIONS;i++) {
    sub_rm_table[i] = create_sub_rm(ram_ds,curr_offset,REGION_SIZE);
    ASSERT(sub_rm_table[i].valid(),"invalid sub-rm ds");
    curr_offset += REGION_SIZE;
  }
  
  PLOG("All regions (%u) sub-rm dataspaces created OK.",NUM_REGIONS);

  /* attach VM spaces */
  for(unsigned i=0;i<NUM_REGIONS;i++) {
    p_table[i] = attach_to_sub_rm(sub_rm_table[i]);
    ASSERT(p_table[i],"attach_to_sub_rm returned NULL");
    PLOG("attached VM space to region (%u)",i);
  }

  PLOG("All sub-rms (%u) attached to OK.",NUM_REGIONS);


  /* test out regions */
  for(unsigned i=0;i<NUM_REGIONS;i++) {
    memset(p_table[i],i,REGION_SIZE);
  }

  for(unsigned i=0;i<NUM_REGIONS;i++) {
    char * p = (char *) p_table[i];
    for(unsigned j=0;j<REGION_SIZE;j++) {
      ASSERT(p[j]==i, "invalid data");
    }
    PLOG("checked region (%u)",i);
  }

  PLOG("All attached sub-rms (%u) checked data OK.",NUM_REGIONS);

  Genode::sleep_forever();
  return 0;
}
