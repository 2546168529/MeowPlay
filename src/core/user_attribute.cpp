#include "user_attribute.h"
#include "../util/log.h"
#include <sstream>

using std::string;
using std::stringstream;

/*
** ��ȡ�û���������
** @param _user �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param read_data ��ѯ��Ϻ���ô˺�������һ�������ǲ�ѯ�����¼����ָ��Ҫ��ȡ�ļ�¼�������ͷţ����ڶ���������Ҫ��ȡ���ֶ�����
** @return �Ƿ��ѯ�ɹ� */
bool mp::read_user_attribute(int64_t _user, string _properties, void (*read_data)(sqlite3_stmt*, int))
{
	mp::lock_db.lock();
	bool status_flag = false;

	int rc = 0;

	sqlite3_stmt *stmt = nullptr;
	
	rc = sqlite3_prepare_v2(mp::connect, "SELECT attribute_value FROM db_player_data.basic_attribute WHERE user_id=@user_id AND attribute_name=@attribute_name LIMIT 1", -1, &stmt, nullptr);
	
	if(rc == SQLITE_OK) 
	{
		sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@user_id"), _user);
		sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
		if(sqlite3_step(stmt) == SQLITE_ROW)
		{
			read_data(stmt, 0);
			status_flag = true;
		}
		else
		{
			/* δ���ҵ���¼��Ӧ�����־ */
			log(log::wran, "user_attribute", "read_user_attribute_int32") << "��ѯIDΪ" << _user << "����ҵ�" << _properties << "����ʱ���ָ����û��������Եļ�¼" << log::push;
			status_flag = false;
		}
		
	}
	else
	{
		/* sqlite3_prepare_v2ʧ�ܣ�Ӧ�����־ */
		log(log::error, "user_attribute", "read_user_attribute_int32") << "��ѯIDΪ" << _user << "����ҵ�" << _properties << "����ʱ��������" << sqlite3_errmsg(mp::connect) << log::push;
		status_flag = false;
	}
	
	sqlite3_finalize(stmt);
	mp::lock_db.unlock();
	return status_flag;
}

/*
** д�û���������
** @param _user �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param bind_new_data ��ʼ�޸Ļ���������ʱ������ô˺�������һ��������sqlite3_stmt���ڶ������������ݲ���������
** @return �Ƿ�д��ɹ� */
bool mp::write_user_attribute(int64_t _user, string _properties, void (*bind_new_data)(sqlite3_stmt*, int))
{
	mp::lock_db.lock();
	bool status_flag = false;

	int rc = 0;

	sqlite3_stmt *quer_stmt = nullptr, *exec_stmt = nullptr;
	
	rc = sqlite3_prepare_v2(mp::connect, "SELECT attribute_value FROM db_player_data.basic_attribute WHERE user_id=@user_id AND attribute_name=@attribute_name LIMIT 1", -1, &quer_stmt, nullptr);
	
	if(rc == SQLITE_OK) 
	{
		sqlite3_bind_int64(quer_stmt, sqlite3_bind_parameter_index(quer_stmt, "@user_id"), _user);
		sqlite3_bind_text(quer_stmt, sqlite3_bind_parameter_index(quer_stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
		rc = sqlite3_step(quer_stmt);
		
		if(rc == SQLITE_ROW)
		{
			/* ���ҵ���¼��ִ���޸Ĳ��� */
			rc = sqlite3_prepare_v2(mp::connect, "UPDATE db_player_data.basic_attribute SET attribute_value = @attribute_value WHERE user_id = @user_id AND attribute_name = @attribute_name", -1, &exec_stmt, nullptr);
		}
		else
		{
			/* δ���ҵ���¼��ִ�в������ */
			rc = sqlite3_prepare_v2(mp::connect, "INSERT INTO db_player_data.basic_attribute(user_id, attribute_name, attribute_value) VALUES (@user_id, @attribute_name, @attribute_value)", -1, &exec_stmt, nullptr);
		}

		if (rc == SQLITE_OK)
		{
			sqlite3_bind_int64(exec_stmt, sqlite3_bind_parameter_index(exec_stmt, "@user_id"), _user);
			sqlite3_bind_text(exec_stmt, sqlite3_bind_parameter_index(exec_stmt, "@attribute_name"), _properties.c_str(), _properties.size(), SQLITE_TRANSIENT);
			bind_new_data(exec_stmt, sqlite3_bind_parameter_index(exec_stmt, "@attribute_value"));
			rc = sqlite3_step(exec_stmt);
			if(rc == SQLITE_DONE)
			{
				status_flag = true;
			}
			else
			{
				/* ִ�в�����޸����ʧ�ܣ�Ӧ�����־ */
				status_flag = false;
				log(log::error, "user_attribute", "read_user_attribute_text") << "д��IDΪ" << _user << "����ҵ�" << _properties << "����ʱ�ڽ��ж�д����ʱ��������" << sqlite3_errmsg(mp::connect) << log::push;
			}
			
		}
		else
		{
			/* ׼��������޸����ʧ�ܣ�Ӧ�����־ */
			status_flag = false;
			log(log::error, "user_attribute", "read_user_attribute_text") << "д��IDΪ" << _user << "����ҵ�" << _properties << "����ʱ��׼��ִ�����ʱ��������" << sqlite3_errmsg(mp::connect) << log::push;
		}
		
	}
	else
	{
		/* ׼����ѯ���ʧ�ܣ�Ӧ�����־ */
		status_flag = false;
		log(log::error, "user_attribute", "read_user_attribute_text") << "д��IDΪ" << _user << "����ҵ�" << _properties << "����ʱ�ڲ�ѯ�����ָ�������Ƿ����ʱ��������" << sqlite3_errmsg(mp::connect) << log::push;
	}
	
	sqlite3_finalize(quer_stmt);
	sqlite3_finalize(exec_stmt);
	mp::lock_db.unlock();
	return status_flag;
}
