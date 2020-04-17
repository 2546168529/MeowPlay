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
mp::database::manage mp::db_manage;

const std::string mp::nullstr("");

using std::string;
using std::stringstream;

/**
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
	
	if (db_manage.open(_game_db.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE))
	{
		stringstream ss;
		ss << "ATTACH DATABASE '" << _user_db << "' AS db_player_data";
		
		if (db_manage.exec_noquery(ss.str().data()))
		{
			return init_database_struct();
		}

		db_manage.close();
	}

	return false;
}

/**
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

	bool exec_status = db_manage.exec_noquery(sql.str().data());
	if (!exec_status)
	{
		log(log::error, "core-basic", "init_database") << "��ʼ�����ݿ�ṹʧ��:" << db_manage.errstr() << log::push;
		app_info.status_runtime = false;
		db_manage.close();
	}
	else
	{
		app_info.status_runtime = true;
	}

	lock_write.unlock();
	
	return exec_status;
}
