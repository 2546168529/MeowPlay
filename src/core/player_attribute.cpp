#include "player_attribute.h"
#include "../util/log.h"

using std::string;

/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��player_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default �����ѯʧ�ܣ������ص�Ĭ��ֵ
** @return ��ѯ���
** @lock �˺��������ж��������Ҳ������κ�ȫ����Դ������ */
template <class __pro, class __dt>
inline __dt read_player_attribute(int64_t _player_id, const __pro _properties, const __dt _default)
{
	__dt result = _default;
	
	/* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return result;
	/* ������ǰ״̬Ϊ��ֹ�����ݿ⣬ȡ������ */
	if (mp::app_info.status_ban_read_database)
		return result;

	mp::database::stmt stmt = mp::db_manage.prepare(
		"SELECT attribute_value FROM db_play_data.player_attribute WHERE player_id=@player_id AND attribute_name=@attribute_name LIMIT 1", 
		{"@player_id", "@attribute_name"}, 
		_player_id, _properties);
	if (stmt.open_success())
	{
		if (stmt.step() == SQLITE_ROW)
		{
			stmt.column(0, result);
		}
		else
		{
			/* δ���ҵ���¼��Ӧ�����־ */
			mp::log(mp::log::wran, "player_attribute", "read_player_attribute_int32") << "��ѯIDΪ" << _player_id << "����ҵ�" << _properties << "����ʱ���ָ����û��������Եļ�¼" << mp::log::push;
		}
	}
	else
	{
		/* sqlite3_prepare_v2ʧ�ܣ�Ӧ�����־ */
		mp::log(mp::log::error, "player_attribute", "read_player_attribute_int32") << "��ѯIDΪ" << _player_id << "����ҵ�" << _properties << "����ʱ��������" << mp::db_manage.errstr() << mp::log::push;
	}

	return result;
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param bind_new_data ��ʼ�޸Ļ���������ʱ������ô˺�������һ��������sqlite3_stmt���ڶ������������ݲ���������
** @return �Ƿ�д��ɹ�
** @lock �˺���������д���������������ȫ����Դ���������ݿ�д��������ִ���ڼ䲻��������д��Ϊ */
template <class __pro, class __dt>
inline bool write_player_attribute(int64_t _player_id, const __pro _properties, const __dt _data)
{
	/* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return false;
	/* ������ǰ״̬Ϊ��ֹд���ݿ⣬ȡ������ */
	if (mp::app_info.status_ban_write_database)
		return false;

	mp::lock_write.lock();
	bool status_flag = false;

	mp::database::stmt quer_stmt = mp::db_manage.prepare(
		"SELECT attribute_value FROM db_play_data.player_attribute WHERE player_id=@player_id AND attribute_name=@attribute_name LIMIT 1", 
		{"@player_id", "@attribute_name"}, 
		_player_id, _properties);

	if (quer_stmt.open_success())
	{

		const char *exec_sql;
		if (quer_stmt.step() == SQLITE_ROW)
		{
			/* ���ҵ���¼��ִ���޸Ĳ��� */
			exec_sql = "UPDATE db_play_data.player_attribute SET attribute_value = @attribute_value WHERE player_id = @player_id AND attribute_name = @attribute_name";
		}
		else
		{
			/* δ���ҵ���¼��ִ�в������ */
			exec_sql = "INSERT INTO db_play_data.player_attribute(player_id, attribute_name, attribute_value) VALUES (@player_id, @attribute_name, @attribute_value)";
		}
		
		status_flag = mp::db_manage.exec_noquery(exec_sql, {"@player_id", "@attribute_name", "@attribute_value"}, _player_id, _properties, _data);
		if (!status_flag)
		{
			mp::log(mp::log::error, "player_attribute", "read_player_attribute_text") << "д��IDΪ" << _player_id << "����ҵ�" << _properties << "����ʱ�ڽ��ж�д����ʱ��������" << mp::db_manage.errstr() << mp::log::push;
		}
	}
	else
	{
		/* ׼����ѯ���ʧ�ܣ�Ӧ�����־ */
		status_flag = false;
		mp::log(mp::log::error, "player_attribute", "read_player_attribute_text") << "д��IDΪ" << _player_id << "����ҵ�" << _properties << "����ʱ�ڲ�ѯ�����ָ�������Ƿ����ʱ��������" << mp::db_manage.errstr() << mp::log::push;
	}

	mp::lock_write.unlock();
	return status_flag;
}


/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
int32_t mp::read_player_attribute_int32(int64_t _player_id, string _properties, int32_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
int64_t mp::read_player_attribute_int64(int64_t _player_id, string _properties, int64_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
double mp::read_player_attribute_double(int64_t _player_id, string _properties, double _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
string mp::read_player_attribute_text(int64_t _player_id, string _properties, string _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}


/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
int32_t mp::read_player_attribute_int32(int64_t _player_id, const char* _properties, int32_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
int64_t mp::read_player_attribute_int64(int64_t _player_id, const char* _properties, int64_t _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
double mp::read_player_attribute_double(int64_t _player_id, const char* _properties, double _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** ��ȡ�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ��ȡ�������������
** @param _default ��ѯʧ�ܺ�Ĭ�Ϸ��ص�����
** @return ��ѯ��� */
string mp::read_player_attribute_text(int64_t _player_id, const char* _properties, string _default)
{
	return read_player_attribute(_player_id, _properties, _default);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_int32(int64_t _player_id, std::string _properties, int32_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_int64(int64_t _player_id, std::string _properties, int64_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_double(int64_t _player_id, std::string _properties, double _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_text(int64_t _player_id, std::string _properties, string _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_text(int64_t _player_id, std::string _properties, const char* _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_int32(int64_t _player_id, const char* _properties, int32_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_int64(int64_t _player_id, const char* _properties, int64_t _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_double(int64_t _player_id, const char* _properties, double _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_text(int64_t _player_id, const char* _properties, string _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

/**
** д�û���������
** @param _player_id �û�ID����IDΪ�����ע��ʱϵͳ�Զ������id����¼��user_register��
** @param _properties ��Ҫ�޸ĵĵ������������
** @param _data Ҫд�������
** @return �Ƿ�д��ɹ� */
bool mp::write_player_attribute_text(int64_t _player_id, const char* _properties, const char* _data)
{
	return write_player_attribute(_player_id, _properties, _data);
}

