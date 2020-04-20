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
** ��֤������ݿ�(db_play_data)�Ѹ��� */
mp::database::manage mp::db_manage;

const std::string mp::nullstr("");

using std::string;
using std::stringstream;

/**
** ��ʼ�����ݿ�
** @param _base_db ��Ϸ�������ݿ�·�������ڱ�����Ϸ����
** @param _player_db ������ݿ�·���������ڴ洢�������
** @return ��Ϊtrue����֤��Ϸ���ݿ��Ѵ򿪣���������ݿ�(db_play_data)�Ѹ��� */
bool mp::init_database(string _base_db, string _player_db)
{

	if (_base_db.find('\'') != string::npos && _player_db.find('\'') != string::npos)
	{
		/* ���ݿ�·���ﲻ��Я��'���ţ��˴�Ӧ�������־ */
		log(log::error, "core-basic", "init_database") << "���ݿ�·���ﲻ��Я��'����" << log::push;
		return false;
	}
	
	if (db_manage.open(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE))
	{
		stringstream ss;
		ss << "ATTACH DATABASE '" << _player_db << "' AS db_play_data;";
		ss << "ATTACH DATABASE '" << _base_db << "' AS db_base_data;";
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
	
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.player_register(player_id INTEGER PRIMARY KEY AUTOINCREMENT,bind_info VARCHAR);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.player_attribute(player_id INTEGER, attribute_name VARCHAR,attribute_value VARCHAR);";
	
	/* ��Ʒϵͳ */
	sql << "CREATE TABLE IF NOT EXISTS db_base_data.items_info (item_base_id INTEGER PRIMARY KEY AUTOINCREMENT,item_name VARCHAR,item_type INTEGER,uniqueness INTEGER,base_info VARCHAR);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.items_uuid_base (item_base_id INTEGER,uuid_base INTEGER);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.player_items (item_uuid INTEGER,player_id INTEGER,amount INTEGER,position INTEGER,position_weight INTEGER,other INTEGER);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.items_register (item_uuid INTEGER,item_base_id INTEGER,regist_time DATETIME,time_limit INTEGER,weight INTEGER);";
	sql << "CREATE TABLE IF NOT EXISTS db_play_data.items_attributes (item_uuid INTEGER,attributes_name VARCHAR,attributes_value VARCHAR);";
	
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
