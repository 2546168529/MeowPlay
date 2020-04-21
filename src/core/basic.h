#pragma once

#include <thread>
#include <mutex>

#include <inttypes.h>
#include <string>
#include <atomic>

#include "../util/log.h"
#include "../sqlite/sqlite3.h"
#include "../sqlite/database.h"

namespace mp
{
	
enum Status
{
	status_ok,
	status_error,
	status_nonexistent,
	status_exist,
	status_ban,
	status_query_error,
	status_exec_error,
	status_begin_transaction_error,
	status_json_is_null
};

/*
	** ������Ϣ�ṹ
	** ���ڴ˽ṹ������ȫ����Դ�����Գ�Ա������Ҫԭ�Ӳ��� */
struct app_info_struct
{
	//�����Ƿ���������
	std::atomic_bool status_runtime = false;
};
extern app_info_struct app_info;

extern const std::string nullstr;

extern std::recursive_mutex lock_write;
extern mp::database::manage db_manage;

bool init_database(std::string _game_db, std::string _user_db);
bool init_database_struct();

} // namespace mp
