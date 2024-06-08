/*
 * stack_mem.h
 *
 *  Created on: 2024. 6. 10.
 *      Author: hp878
 */

#ifndef SRC_LIB_TMWSCL_TMWTARG_STACK_MEM_H_
#define SRC_LIB_TMWSCL_TMWTARG_STACK_MEM_H_

#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/dnp/sdnpdata.h"

/*
 * DNP Stack Memory allocated in External SRAM(ZONE 6)
 */

#if !TMWCNFG_USE_DYNAMIC_MEMORY
#pragma DATA_SECTION(dnpmem_chnls,          "DNP_Stack")
#if TMWCNFG_SUPPORT_DIAG
#pragma DATA_SECTION(dnpmem_linkFrames,     "DNP_Stack")
#endif
#pragma DATA_SECTION(dnpmem_linkSessions,   "DNP_Stack")
#pragma DATA_SECTION(dnpmem_linkContexts,   "DNP_Stack")
#pragma DATA_SECTION(dnpmem_tprtSessions,   "DNP_Stack")
#pragma DATA_SECTION(dnpmem_tprtContexts,   "DNP_Stack")
#pragma DATA_SECTION(dnpmem_txDatas,        "DNP_Stack")
#endif

#pragma DATA_SECTION(sdnpmem_sSesns,                "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_asduBuffers,           "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_object2Events,         "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_object4Events,         "DNP_Stack")
#if SDNPDATA_SUPPORT_OBJ11
#pragma DATA_SECTION(sdnpmem_object11Events,        "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_OBJ13
#pragma DATA_SECTION(sdnpmem_object13Events,        "DNP_Stack")
#endif
#pragma DATA_SECTION(sdnpmem_object22Events,        "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_object23Events,        "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_object32Events,        "DNP_Stack")
#if SDNPDATA_SUPPORT_OBJ42
#pragma DATA_SECTION(sdnpmem_object42Events,        "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_OBJ43
#pragma DATA_SECTION(sdnpmem_object43Events,        "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_OBJ70
#pragma DATA_SECTION(sdnpmem_object70CtrlBlocks,    "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_OBJ88
#pragma DATA_SECTION(sdnpmem_object88EventBlocks,   "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_OBJ111
#pragma DATA_SECTION(sdnpmem_object111Events,       "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_OBJ113
#pragma DATA_SECTION(sdnpmem_object113Events,       "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_OBJ120
#pragma DATA_SECTION(sdnpmem_object120Events,       "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_object122Events,       "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_authInfos,             "DNP_Stack")
#if SDNPCNFG_SUPPORT_SA_VERSION5
#pragma DATA_SECTION(sdnpmem_authUsers,             "DNP_Stack")
#endif
#endif
#if TMWCNFG_USE_SIMULATED_DB
#pragma DATA_SECTION(sdnpmem_simDbases,             "DNP_Stack")
#if SDNPCNFG_USER_MANAGED_EVENTS
#pragma DATA_SECTION(sdnpmem_simUMEvents,           "DNP_Stack")
#endif
#pragma DATA_SECTION(sdnpmem_simDatasetProtos,      "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_simDatasetDescrs,      "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_DATASETS
#pragma DATA_SECTION(sdnpmem_simAuthErrors,         "DNP_Stack")
#pragma DATA_SECTION(sdnpmem_simAuthUsers,          "DNP_Stack")
#endif
#if SDNPDATA_SUPPORT_XML2
#pragma DATA_SECTION(sdnpmem_xmlConfigs,            "DNP_Stack")
#endif



#endif /* SRC_LIB_TMWSCL_TMWTARG_STACK_MEM_H_ */
