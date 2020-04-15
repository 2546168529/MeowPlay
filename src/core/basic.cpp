#include "basic.h"
#include <sstream>

/* 
** �������ݿ��д����
** ��Ҫ��;��
** ���Ա�����������ʱ����������д����
** ���Ա���ͬʱ����д����� */
std::mutex mp::lock_db;

/*
** SQLITE���ݿ����Ӿ��
** ��֤������ݿ�(db_player_data)�Ѹ��� */
sqlite3* mp::connect = nullptr;

using std::string;
using std::stringstream;

/*
** ��ʼ�����ݿ�
** @param _game_db ��Ϸ���ݿ�·�������ڱ�����Ϸ����
** @param _user_db ������ݿ�·���������ڴ洢�������
** @return ��Ϊtrue����֤��Ϸ���ݿ��Ѵ򿪣���������ݿ�(db_player_data)�Ѹ��� */
bool mp::init_database(string _game_db, string _user_db)
{

	if (_user_db.find('\'') != string::npos)
	{
		/* ���ݿ�·���ﲻ��Я��'���ţ��˴�Ӧ�������־ */
		log(log::error, "core-basic", "init_database") << "���ݿ�·���ﲻ��Я��'����" << log::push;
		return false;
	}

	int rc;
	rc = sqlite3_open_v2(_game_db.c_str(), &connect, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	if (rc == SQLITE_OK)
	{
		sqlite3_stmt* stmt;
		stringstream ss;
		ss << "ATTACH DATABASE '" << _user_db << "' AS db_player_data";
		
		rc = sqlite3_prepare_v2(connect, ss.str().c_str(), -1, &stmt, 0);
		if (rc == SQLITE_OK)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			if (rc == SQLITE_DONE)
			{
				return true;
			}
		}
		sqlite3_finalize(stmt);
		sqlite3_close(connect);
	}

	return false;
}

/*
** ��ʼ�����ݿ�ṹ
** @return ��Ϊtrue����֤��Ϸ���ݿ���������ݿ�ṹ���� */
bool mp::init_database_struct()
{
	stringstream sql;
	//��ҵǼǱ�Ϊÿ���Ǽǵ���ҷ���Ψһ��ID���Լ���
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.user_register(id INTEGER PRIMARY KEY AUTOINCREMENT, bind_info VARCHAR);";
	
	//�����û��������Ա�
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.basic_attribute(user_id INTEGER,attribute_name VARCHAR,attribute_value VARCHAR);";
	
	char* error;
	int rc = sqlite3_exec(mp::connect, sql.str().c_str(), nullptr, nullptr, &error);
	if (rc != SQLITE_OK)
	{
		//�˴�Ӧ���������Ϣ
		log(log::error, "core-basic", "init_database") << "��ʼ�����ݿ�ṹʧ��:" << sqlite3_errmsg(mp::connect) << log::push;
	}

	sqlite3_free(error);

	return rc == SQLITE_OK;
}