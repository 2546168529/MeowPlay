#include "basic.h"
#include <sstream>

/*
** ��������״̬ */
mp::app_info_struct mp::app_info;

/* 
** ���ݿ�д��
** ��Ҫ��;��
** ���Ա�����������ʱ��������д����
** ���Ա���ͬʱ����д����� */
std::mutex mp::lock_write;

/*
** SQLITE���ݿ����Ӿ��
** ��Ϊ����ȫ����Դ��������������ʱ�ı���ֵ
** ��֤������ݿ�(db_player_data)�Ѹ��� */
sqlite3* mp::connect = nullptr;
mp::database::manage mp::connect_manage;

const std::string mp::nullstr("");

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
	connect_manage.connect_ptr(connect);

	if (rc == SQLITE_OK)
	{
		stringstream ss;
		ss << "ATTACH DATABASE '" << _user_db << "' AS db_player_data";
		
		if (connect_manage.exec_noquery(ss.str().data()))
		{
			return init_database_struct();
		}

		connect_manage.close();
		connect = nullptr;
	}

	return false;
}

/*
** ��ʼ�����ݿ�ṹ
** @return ��Ϊtrue����֤��Ϸ���ݿ���������ݿ�ṹ���� */
bool mp::init_database_struct()
{
	lock_write.lock();
	stringstream sql;
	//��ҵǼǱ�Ϊÿ���Ǽǵ���ҷ���Ψһ��ID���Լ���
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.player_register(player_id INTEGER PRIMARY KEY AUTOINCREMENT, bind_info VARCHAR);";
	
	//�����û��������Ա�
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.player_attribute(player_id INTEGER,attribute_name VARCHAR,attribute_value VARCHAR);";
	
	//������ұ�����
	sql << "CREATE TABLE IF NOT EXISTS db_player_data.player_items (player_item_id INTEGER PRIMARY KEY AUTOINCREMENT,player_id INTEGER,item_id INTEGER,amount INTEGER,position_weight INTEGER);";

	bool exec_status = connect_manage.exec_noquery(sql.str().data());
	if (!exec_status)
	{
		log(log::error, "core-basic", "init_database") << "��ʼ�����ݿ�ṹʧ��:" << sqlite3_errmsg(mp::connect) << log::push;
		app_info.status_runtime = false;
		connect_manage.close();
		connect = nullptr;
	}
	else
	{
		app_info.status_runtime = true;
	}

	lock_write.unlock();
	
	return exec_status;
}
