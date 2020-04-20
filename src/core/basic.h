#pragma once

#include <thread>
#include <mutex>

#include <inttypes.h>
#include <string>
#include <atomic>

#include "../util/log.h"
#include "../sqlite/sqlite3.h"
#include "../sqlite/database.h"

namespace mp {
	
	/*
	** ������Ϣ�ṹ
	** ���ڴ˽ṹ������ȫ����Դ�����Գ�Ա������Ҫԭ�Ӳ��� */
	struct app_info_struct
	{
		//�����Ƿ���������
		std::atomic_bool status_runtime = false;
		//�Ƿ��ֹ�����ݿ�
		std::atomic_bool status_ban_read_database = false;
		//�Ƿ��ֹд���ݿ�
		std::atomic_bool status_ban_write_database = false;
	};
	extern app_info_struct app_info;

	//ͨ�ò�ѯ���
	enum query_result{
		success,	//�ɹ�
		nonexistent,//������
		error,		//����
		ban			//��ֹ����
	};
	
	extern const std::string nullstr;

	extern std::recursive_mutex lock_write;
	extern mp::database::manage db_manage;

	bool init_database(std::string _game_db, std::string _user_db);
	bool init_database_struct();

}

