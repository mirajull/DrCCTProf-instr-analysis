/* 
 *  Copyright (c) 2020-2021 Md Mirajul Islam. All rights reserved.
 *  Licensed under the MIT License.
 *  See LICENSE file for more information.
 */

/* DrCCTProf Client:
 * instr_analysis.cpp
 *
 * This client will display the top 10 instructions of the 4 instruction categories sorted by 
 * execution times and calling paths in the output file.
 *
 * If you are new to the DynamoRIO Dynamic Binary Instrumentation (DBI) tool platform in
 * general, and DynamoRIO in particular, ensure you understand the method by which
 * instrumentation is added to application code.
 *
 * Remember that instrumentation occurs in two phases, transformation and execution:
 * - Transformation
 *      Instrumentation code is inserted into the application code.
 * - Execution
 *      The application code runs, including the instrumentation code which was inserted
 *      during transformation.
 */

#include <iterator>
#include <vector>
#include <map>
#include <string>

#include "dr_api.h"
#include "drcctlib.h"

#define DRCCTLIB_PRINTF(_FORMAT, _ARGS...) \
    DRCCTLIB_PRINTF_TEMPLATE("instr_analysis", _FORMAT, ##_ARGS)
#define DRCCTLIB_EXIT_PROCESS(_FORMAT, _ARGS...) \
    DRCCTLIB_CLIENT_EXIT_PROCESS_TEMPLATE("instr_analysis", _FORMAT, ##_ARGS)

#ifdef ARM_CCTLIB
#    define OPND_CREATE_CCT_INT OPND_CREATE_INT
#else
#    define OPND_CREATE_CCT_INT OPND_CREATE_INT32
#endif

#define MAX_CLIENT_CCT_PRINT_DEPTH 10
#define TOP_REACH_NUM_SHOW 10

#define CATEGORIES_NUM 4

#define MEMORY_LOAD 0
#define MEMORY_STORE 1
#define CND_BRANCH 2
#define UNCND_BRANCH 3

uint64_t *gloabl_hndl_call_num;

uint64_t *ubr_hndl_call_num;
uint64_t *cbr_hndl_call_num;
uint64_t *ld_hndl_call_num;
uint64_t *str_hndl_call_num;

uint64_t ubr_instr_num=0;
uint64_t cbr_instr_num=0;
uint64_t ld_instr_num=0;
uint64_t str_instr_num=0;

static file_t gTraceFile;

using namespace std;


// Execution
void
InsCount(int32_t slot, bool is_ubr, bool is_cbr, bool is_str, bool is_ld)
{
    void *drcontext = dr_get_current_drcontext();
    context_handle_t cur_ctxt_hndl = drcctlib_get_context_handle(drcontext, slot);
    gloabl_hndl_call_num[cur_ctxt_hndl]++;
    
    if(is_ubr)
    {
	ubr_instr_num++;
        ubr_hndl_call_num[cur_ctxt_hndl]++;
    }
    if(is_cbr)
    {
	cbr_instr_num++;
        cbr_hndl_call_num[cur_ctxt_hndl]++;
    }
    if(is_str)
    {
	str_instr_num++;
        str_hndl_call_num[cur_ctxt_hndl]++;
    }
    if(is_ld)
    {
	ld_instr_num++;
        ld_hndl_call_num[cur_ctxt_hndl]++;
    }
}

// Transformation
void
InsTransEventCallback(void *drcontext, instr_instrument_msg_t *instrument_msg)
{

    instrlist_t *bb = instrument_msg->bb;
    instr_t *instr = instrument_msg->instr;
    int32_t slot = instrument_msg->slot;
    int32_t type = 0;
    
    int32_t is_ubr = 0;
    int32_t is_cbr = 0;
    int32_t is_ld = 0;
    int32_t is_str = 0;

    if(instr_is_ubr(instr)) 
    {
	is_ubr = 1;
    }
    
    if(instr_is_cbr(instr))
    {
	is_cbr = 1;
    }

    if(instr_writes_memory(instr))
    {
	is_str = 1;
    }
    
    if(instr_reads_memory(instr))
    {
	is_ld = 1;
    }

    dr_insert_clean_call(drcontext, bb, instr, (void *)InsCount, false, 5, OPND_CREATE_CCT_INT(slot), OPND_CREATE_CCT_INT(is_ubr), OPND_CREATE_CCT_INT(is_cbr), 
OPND_CREATE_CCT_INT(is_str), OPND_CREATE_CCT_INT(is_ld));
}

static inline void
InitGlobalBuff()
{
    gloabl_hndl_call_num = (uint64_t *)dr_raw_mem_alloc(
        CONTEXT_HANDLE_MAX * sizeof(uint64_t), DR_MEMPROT_READ | DR_MEMPROT_WRITE, NULL);
    if (gloabl_hndl_call_num == NULL) {
        DRCCTLIB_EXIT_PROCESS(
            "init_global_buff error: dr_raw_mem_alloc fail gloabl_hndl_call_num");
    }
    
    ubr_hndl_call_num = (uint64_t *)dr_raw_mem_alloc(
        CONTEXT_HANDLE_MAX * sizeof(uint64_t), DR_MEMPROT_READ | DR_MEMPROT_WRITE, NULL);
    if (ubr_hndl_call_num == NULL) {
        DRCCTLIB_EXIT_PROCESS(
            "init_ubr_buff error: dr_raw_mem_alloc fail ubr_hndl_call_num");
    }
 
    cbr_hndl_call_num = (uint64_t *)dr_raw_mem_alloc(
        CONTEXT_HANDLE_MAX * sizeof(uint64_t), DR_MEMPROT_READ | DR_MEMPROT_WRITE, NULL);
    if (cbr_hndl_call_num == NULL) {
        DRCCTLIB_EXIT_PROCESS(
            "init_cbr_buff error: dr_raw_mem_alloc fail cbr_hndl_call_num");
    }

    str_hndl_call_num = (uint64_t *)dr_raw_mem_alloc(
        CONTEXT_HANDLE_MAX * sizeof(uint64_t), DR_MEMPROT_READ | DR_MEMPROT_WRITE, NULL);
    if (str_hndl_call_num == NULL) {
        DRCCTLIB_EXIT_PROCESS(
            "init_str_buff error: dr_raw_mem_alloc fail str_hndl_call_num");
    }

    ld_hndl_call_num = (uint64_t *)dr_raw_mem_alloc(
        CONTEXT_HANDLE_MAX * sizeof(uint64_t), DR_MEMPROT_READ | DR_MEMPROT_WRITE, NULL);
    if (ld_hndl_call_num == NULL) {
        DRCCTLIB_EXIT_PROCESS(
            "init_ld_buff error: dr_raw_mem_alloc fail ld_hndl_call_num");
    }
}

static inline void
FreeGlobalBuff()
{
    dr_raw_mem_free(gloabl_hndl_call_num, CONTEXT_HANDLE_MAX * sizeof(uint64_t));
    dr_raw_mem_free(ubr_hndl_call_num, CONTEXT_HANDLE_MAX * sizeof(uint64_t));
    dr_raw_mem_free(cbr_hndl_call_num, CONTEXT_HANDLE_MAX * sizeof(uint64_t));
    dr_raw_mem_free(str_hndl_call_num, CONTEXT_HANDLE_MAX * sizeof(uint64_t));
    dr_raw_mem_free(ld_hndl_call_num, CONTEXT_HANDLE_MAX * sizeof(uint64_t));
}

static void
ClientInit(int argc, const char *argv[])
{
    char name[MAXIMUM_FILEPATH] = "";
    DRCCTLIB_INIT_LOG_FILE_NAME(
        name, "instr_analysis", "out");
    DRCCTLIB_PRINTF("Creating log file at:%s", name);

    gTraceFile = dr_open_file(name, DR_FILE_WRITE_OVERWRITE | DR_FILE_ALLOW_LARGE);
    DR_ASSERT(gTraceFile != INVALID_FILE);

    InitGlobalBuff();
    drcctlib_init(DRCCTLIB_FILTER_ALL_INSTR, INVALID_FILE, InsTransEventCallback, false);
}

typedef struct _output_format_t {
    context_handle_t handle;
    uint64_t count;
} output_format_t;

static void
ClientExit(void)
{
    output_format_t *output_list = 
		(output_format_t *)dr_global_alloc(TOP_REACH_NUM_SHOW *	sizeof(output_format_t));
    context_handle_t max_ctxt_hndl = drcctlib_get_global_context_handle_num();
    output_format_t temp;
    for (int32_t k = 0; k < CATEGORIES_NUM; k++) {
	for (int32_t i = 0; i < TOP_REACH_NUM_SHOW; i++) {
	    output_list[i].handle = 0;
	    output_list[i].count = 0;
	}
	if (k == MEMORY_LOAD) {
	   dr_fprintf(gTraceFile, "MEMORY LOAD : %d\n", ld_instr_num);
	   for (context_handle_t i = 0; i < max_ctxt_hndl; i++) {
	       if (ld_hndl_call_num[i] <= 0) {
	          continue;
	       }
	       if (ld_hndl_call_num[i] > output_list[0].count) {
	          uint64_t min_count = ld_hndl_call_num[i];
	          int32_t min_idx = 0;
	          for (int32_t j = 1; j < TOP_REACH_NUM_SHOW; j++) {
		      if (output_list[j].count < min_count) {
		         min_count = output_list[j].count;
		         min_idx = j;
		      }
	          }
	          output_list[0].count = min_count;
	          output_list[0].handle = output_list[min_idx].handle;
	          output_list[min_idx].count = ld_hndl_call_num[i];
	          output_list[min_idx].handle = i;
	       }
	   }
	} else if (k == MEMORY_STORE) {
	   dr_fprintf(gTraceFile, "MEMORY STORE : %d\n", str_instr_num);
           for (context_handle_t i = 0; i < max_ctxt_hndl; i++) {
	       if (str_hndl_call_num[i] <= 0) {
	          continue;
	       }
	       if (str_hndl_call_num[i] > output_list[0].count) {
	          uint64_t min_count = str_hndl_call_num[i];
	          int32_t min_idx = 0;
	          for (int32_t j = 1; j < TOP_REACH_NUM_SHOW; j++) {
		      if (output_list[j].count < min_count) {
		         min_count = output_list[j].count;
		         min_idx = j;
		      }
	          }
	          output_list[0].count = min_count;
	          output_list[0].handle = output_list[min_idx].handle;
	          output_list[min_idx].count = str_hndl_call_num[i];
	          output_list[min_idx].handle = i;
	       }
	   }
	} else if (k == CND_BRANCH) {
	   dr_fprintf(gTraceFile, "CONDITIONAL BRANCHES : %d\n", cbr_instr_num);
           for (context_handle_t i = 0; i < max_ctxt_hndl; i++) {
	       if (cbr_hndl_call_num[i] <= 0) {
	          continue;
	       }
	       if (cbr_hndl_call_num[i] > output_list[0].count) {
	          uint64_t min_count = cbr_hndl_call_num[i];
	          int32_t min_idx = 0;
	          for (int32_t j = 1; j < TOP_REACH_NUM_SHOW; j++) {
		      if (output_list[j].count < min_count) {
		         min_count = output_list[j].count;
		         min_idx = j;
		      }
	          }
	          output_list[0].count = min_count;
	          output_list[0].handle = output_list[min_idx].handle;
	          output_list[min_idx].count = cbr_hndl_call_num[i];
	          output_list[min_idx].handle = i;
	       }
	   }
	} else if (k == UNCND_BRANCH) {
	   dr_fprintf(gTraceFile, "UNCONDITIONAL_BRANCHES : %d\n", ubr_instr_num);
           for (context_handle_t i = 0; i < max_ctxt_hndl; i++) {
	       if (ubr_hndl_call_num[i] <= 0) {
	          continue;
	       }
	       if (ubr_hndl_call_num[i] > output_list[0].count) {
	          uint64_t min_count = ubr_hndl_call_num[i];
	          int32_t min_idx = 0;
	          for (int32_t j = 1; j < TOP_REACH_NUM_SHOW; j++) {
		      if (output_list[j].count < min_count) {
		         min_count = output_list[j].count;
		         min_idx = j;
		      }
	          }
	          output_list[0].count = min_count;
	          output_list[0].handle = output_list[min_idx].handle;
	          output_list[min_idx].count = ubr_hndl_call_num[i];
	          output_list[min_idx].handle = i;
	       }
	   }
	} 
	
	for (int32_t i = 0; i < TOP_REACH_NUM_SHOW; i++) {
	    for (int32_t j = i; j < TOP_REACH_NUM_SHOW; j++) {
	        if (output_list[i].count < output_list[j].count) {
		   temp = output_list[i];
		   output_list[i] = output_list[j];
		   output_list[j] = temp;
	        }
	    }
	}
	for (int32_t i = 0; i < TOP_REACH_NUM_SHOW; i++) {
	    if (output_list[i].handle == 0) {
	       break;
	    }
	    dr_fprintf(gTraceFile, "[NO. %d]", i + 1);
	    dr_fprintf(gTraceFile, "Ins Call times %d\n", output_list[i].count);
	    dr_fprintf(gTraceFile, "=============================================================================================\n");
	    drcctlib_print_backtrace(gTraceFile, output_list[i].handle, true, true, -1);
	    dr_fprintf(gTraceFile, "=============================================================================================\n");
	    dr_fprintf(gTraceFile, "\n\n");
	}
    }
    dr_global_free(output_list, TOP_REACH_NUM_SHOW * sizeof(output_format_t));
    FreeGlobalBuff();
    drcctlib_exit();
    dr_close_file(gTraceFile);
}

#ifdef __cplusplus
extern "C" {
#endif

DR_EXPORT void
dr_client_main(client_id_t id, int argc, const char *argv[])
{
    dr_set_client_name("DrCCTProf Client 'instr_analysis'",
                       "https://drcctprof.readthedocs.io/");

    ClientInit(argc, argv);
    dr_register_exit_event(ClientExit);
}

#ifdef __cplusplus
}
#endif

