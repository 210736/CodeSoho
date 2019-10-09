#pragma once
#include "stdafx.h"

#ifdef _DEBUG

	#ifdef X64
		#pragma comment(lib,"..\\vsout\\ecore\\debug_x64\\ecore.lib")
	#else
		#pragma comment(lib,"..\\vsout\\ecore\\debug\\ecore.lib")
	#endif

#else
#ifdef X64
#pragma comment(lib,"..\\vsout\\ecore\\release_X64\\ecore.lib")
#else
#pragma comment(lib,"..\\vsout\\ecore\\release\\ecore.lib")
#endif

#endif

// 基本逻辑模型
#include "binder.h"
#include "factory.h"
#include "singleton.h"
#include "list_ts.h"
#include "map_ts.h"
#include "safe_queue.h"
#include "simple_map.h"
#include "simple_list.h"
#include "msg_queue_ts.h"
#include "msg_queue.h"
#include "safe_tiny_queue.h"
#include "ptr_queue.h"
#include "wan_queue.h"

/// 对外标准接口
#include "error.h"
#include "log.h"
#include "dump.h"
#include "crash_log.h"
#include "new_protect.h"	
#include "mem_cache.h"
#include "net_define.h"
#include "net_util.h"
#include "ping.h"
#include "lan_server.h"
#include "lan_client.h"
#include "broadcast.h"
#include "wan_server.h"
#include "wan_client.h"
#include "http_client.h"
#include "crc.h"
#include "md5.h"
#include "unicode.h"
#include "string_util.h"
#include "bit_util.h"
#include "disk_io.h"
#include "proc.h"
#include "cpu.h"
#include "xml_loader.h"
#include "ini_loader.h"
#include "ssv_loader.h"
#include "filter.h"
#include "vfs.h"
#include "date_time.h"
#include "functor.h"
#include "singleton.h"

#include "Array.h"
#include "dyna_array.h"
#include "simple_vector.h"
#include "bit_array.h"
#include "flag_array.h"
#include "GeomDefine.h"
#include "Ray.h"
#include "Transform.h"
#include "ObjectSpace.h"
#include "csv_file.h"
#include "FArchive.h"
#include "FArchiveAdv.h"
#include "rtti_info.h"
#include "rtti_obj.h"
#include "Shared_Ptr.h"
#include "StlExt.h"
#include "Timer.h"
#include "file_name.h"
#include "fast_memory.h"
#include "none_copyable.h"
#include "TSList.h"
#include "ref_obj.h"
#include "file_util.h"
#include "object_pool.h"
#include "height_map.h"
#include "height_field.h"
#include "bit_map.h"
#include "half_byte_map.h"
#include "speed_tester.h"
#include "super_string.h"
#include "string_table.h"
#include "registry.h"
#include "fast_code.h"
#include "fast_list.h"
#include "rand_num_pool.h"

#include "mutex.h"
#include "thread_mgr.h"
#include "callback.h"
#include "task.h"
#include "executor_fun.h"
#include "executor_obj.h"
#include "executor_task.h"
#include "task_scheduler.h"

#include "symbol.h"

// 共享内存
#include "shared_memory.h"
#include "event.h"
#include "kernel_mutex.h"