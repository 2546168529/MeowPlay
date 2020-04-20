#include "items_system.h"
#include <inttypes.h>
#include "../json/json11.hpp"
#include <iostream>

/**
** ��ȡ��Ʒ������Ϣ
** @param _item_base_id ��Ʒ����ID
** @param _read_result ��ȡ���
** @return Status mp::status_ok �ɹ���mp::status_ban ����ֹʹ�á�mp::status_error ����mp::status_nonexistent ������
*/
mp::Status read_base_items(uint32_t _item_base_id, mp::BaseItemsInfo& _read_result)
{
    /* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return mp::status_ban;

    bool result_flag = false;
    mp::database::stmt query_stmt = mp::db_manage.prepare(
        "SELECT item_name,item_type,uniqueness,base_info FROM db_base_data.items_info WHERE item_base_id=@item_base_id",
        {"@item_base_id"}, _item_base_id);

    if (!query_stmt.open_success())
    {
        return mp::status_error;
    }

    int rc = query_stmt.step();
    if(rc == SQLITE_ROW)
    {
        result_flag = true;
        
        _read_result.item_base_id = _item_base_id;
        query_stmt.column(0, _read_result.item_name);
        query_stmt.column(1, _read_result.item_type);
        _read_result.uniqueness = (query_stmt.column<int32_t>(2) != 0);
        query_stmt.column(3, _read_result.base_info);

        return mp::status_ok;
    }
    else if(rc == SQLITE_DONE)
    {
        return mp::status_nonexistent;
    }
    else
    {
        return mp::status_error;
    }
}

/**
** ���ָ����Ʒ��BaseUUID�������ڳɹ���ȡ������BaseUUID
** @param _item_base_id ��Ʒ����id
** @return ����base_uuid(�ض����ڵ���1)����Ϊ0���ʾ��ȡʧ��
*/
uint32_t get_items_uuid_base(uint32_t _item_base_id)
{
    /* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return 0;
    
    /* �Զ����� */
    std::lock_guard<std::recursive_mutex> lock(mp::lock_write);

    mp::database::stmt query_stmt = mp::db_manage.prepare(
        "SELECT uuid_base FROM db_play_data.items_uuid_base WHERE item_base_id=@item_base_id",
        {"@item_base_id"}, _item_base_id);

    if (!query_stmt.open_success())
    {
        return 0;
    }
    
    int rc = query_stmt.step();
    if(rc == SQLITE_ROW)
    {
        uint32_t uuid_base = 0;
        query_stmt.column(0, uuid_base);
        if(uuid_base != 0 && mp::db_manage.exec_noquery(
            "UPDATE db_play_data.items_uuid_base SET uuid_base=@uuid_base WHERE item_base_id=@item_base_id",
            {"@item_base_id", "@uuid_base"}, _item_base_id, uuid_base + 1))
        {
            return uuid_base;
        }
    }
    else if(rc == SQLITE_DONE)//������
    {
        if(mp::db_manage.exec_noquery(
            "INSERT INTO db_play_data.items_uuid_base(item_base_id,uuid_base) VALUES(@item_base_id,@uuid_base)",
            {"@item_base_id", "@uuid_base"}, _item_base_id, 2))
        {
            return 1;
        }
    }
    
    return 0;
}

/**
** ���������Ʒ��Ϣ
** @param _player_id ���ID
** @param _item_uuid ��ƷUUID��ȫ��ΨһID��
** @param _amount ��Ʒ��������Ϊ����
** @param _position ��Ʒλ�ô���
** @param _position_weight ��Ʒλ��Ȩ�أ���������
** @param _other ��Ʒ�������ԣ��ӵ͵���λ����һλ��ʾ��Ʒ�Ƿ��������ڶ�λ��ʾ����Ʒ�Ƿ�������ҽ���
** @return �Ƿ�ɹ����������Ʒ���ɹ�����mp::status_ok
*/
mp::Status update_player_items(int64_t _player_id, int64_t _item_uuid, int32_t _amount, int32_t _position, int32_t _position_weight, int64_t _other)
{
    bool result_flag = false;

    /* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return mp::status_ban;

	std::lock_guard<std::recursive_mutex> lock(mp::lock_write);

    mp::database::stmt query_stmt = mp::db_manage.prepare(
        "SELECT amount FROM db_play_data.player_items WHERE item_uuid=@item_uuid AND _player_id=@player_id AND position=@position AND other=@other",
        {"@item_uuid", "@player_id", "@position", "@other"},
        _item_uuid, _player_id, _position, _other);

    if(!query_stmt.open_success())
    {
        int rc = query_stmt.step();
        if(rc == SQLITE_ROW)
        {
            //����
            int64_t final_amount = query_stmt.column<int64_t>(0) + _amount;
            if(final_amount > 0)
            {
                //���������������0���޸�����Ϊ��������
                result_flag = mp::db_manage.exec_noquery(
                    "UPDATE db_play_data.player_items SET amount=@amount WHERE item_uuid=@item_uuid,position_weight=@position_weight AND _player_id=@player_id AND position=@position AND other=@other",
                    {"@item_uuid", "@player_id", "@amount", "@position", "@position_weight", "@other"},
                    _item_uuid, _player_id, final_amount, _position, _position_weight, _other);
            }
            else if(final_amount == 0)
            {
                //��������������0��ֱ��ɾ��
                result_flag = mp::db_manage.exec_noquery(
                    "DELETE FROM db_play_data.player_items WHERE item_uuid=@item_uuid AND _player_id=@player_id AND position=@position AND other=@other",
                    {"@item_uuid", "@player_id", "@position", "@other"},
                    _item_uuid, _player_id, _position, _other);
            }
            else
            {
                //��������С��0
                result_flag = false;
            }
        
        }
        else if(rc == SQLITE_DONE)
        {
            //�����Ʒ���ڲ����������Ʒ
            if(_amount <= 0)
            {
                result_flag = false;
            }
            else
            {
                result_flag = mp::db_manage.exec_noquery(
                    "INSERT INTO db_play_data.player_items(item_uuid,player_id,amount,position,position_weight,other) VALUES(@item_uuid,@player_id,@amount,@position,@position_weight,@other)",
                    {"@item_uuid", "@player_id", "@amount", "@position", "@position_weight", "@other"},
                    _item_uuid, _player_id, _amount, _position, _position_weight, _other);
            }
        }
        else
        {
            //ִ��ʧ��
            result_flag = false;
            mp::log(mp::log::error, "items_system", "update_player_items") << "����IDΪ" << _player_id << "����ҵ���Ʒ��ִ�в�ѯʱ��������rc=" << rc << mp::log::push;
        }
        
    }
    else
    {
        result_flag = false;
    }
    
    return result_flag?mp::status_ok:mp::status_error;
}

/**
** ע����Ʒ
** @param _item_uuid ��ע���UUID
** @param _item_base_id ��Ʒ������ϢID
** @return �ɹ�����mp::status_ok��ʧ�ܷ��ش������
*/
//CREATE TABLE IF NOT EXISTS db_play_data.items_register (item_uuid INTEGER,item_base_id INTEGER,regist_time DATETIME,time_limit INTEGER,weight INTEGER);
mp::Status regist_items(int64_t _item_uuid, uint32_t _item_base_id, int64_t time_limit, int64_t weight)
{
    /* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return mp::status_ban;

    std::lock_guard<std::recursive_mutex> lock(mp::lock_write);
    mp::database::stmt query_stmt = mp::db_manage.prepare("SELECT item_base_id FROM db_play_data.items_register WHERE item_uuid=@item_uuid", {"@item_uuid"}, _item_uuid);
    if(!query_stmt.open_success()) 
    {
        return mp::status_query_error;
    }
    int rc = query_stmt.step();
    if(rc == SQLITE_ROW)
    {
        return mp::status_exist;
    }
    else if(rc != SQLITE_DONE)
    {
        return mp::status_query_error;
    }

    if(mp::db_manage.exec_noquery(
        "INSERT INTO db_play_data.items_register(item_uuid,item_base_id,regist_time,time_limit,weight) VALUES(@item_uuid,@item_base_id,datetime('now'),@time_limit,@weight)",
        {"@item_uuid", "@item_base_id", "@time_limit", "@weight"},
        _item_uuid, _item_base_id, time_limit, weight))
    {
        return mp::status_ok;
    }
    else
    {
        return mp::status_exec_error;
    }

}

/**
** ����û���Ʒ
** @param _player_id ���ID
** @param _item_base_id ��Ʒ������ϢID
** @param _amount ��Ʒ��������Ϊ����
** @param _position ��Ʒλ�ô���
** @param _position_weight ��Ʒλ��Ȩ�أ���������
** @param _time_limit ��Ʒ��ʱ��<=0��Ϊ����ʱ��
** @return �Ƿ�ɹ����������Ʒ���ɹ�����mp::status_ok
*/
mp::Status mp::add_player_items(int64_t _player_id, uint32_t _item_base_id, int64_t _amount, int32_t _position, int32_t _position_weight, int64_t _time_limit)
{

    /* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return mp::status_ban;

	std::lock_guard<std::recursive_mutex> lock(mp::lock_write);

    if(mp::db_manage.begin_transaction() == false)
    {
         return mp::status_begin_transaction_error;
    }

    //��ȡ��Ʒ������Ϣ��UUID
    mp::BaseItemsInfo baseItemsInfo;
    int64_t uuid = 0;
    if(read_base_items(_item_base_id, baseItemsInfo) == mp::status_ok)
    {
        uuid = baseItemsInfo.item_base_id;
        //�����Ʒ��ȷ��Ҫ��������ʱ������
        if(baseItemsInfo.uniqueness || _time_limit > 0)
        {
            uint32_t uuid_base = get_items_uuid_base(_item_base_id);
            if (uuid_base == 0)
            {
                return mp::status_error;
            }
            
            uuid = (static_cast<uint64_t>(baseItemsInfo.item_base_id)<<32) + uuid_base;
        }
    }
    else
    {
        mp::db_manage.rollback_transaction();
        return mp::status_error;
    }

    std::string json_err;
    json11::Json json_base_info = json11::Json::parse(baseItemsInfo.base_info, json_err);
    json11::Json json_base = json_base_info["base"].object_items();
    //ע����Ʒ
    mp::Status regist_status = regist_items(uuid, _item_base_id, _time_limit, json_base["weight"].int_value());
    if(regist_status != mp::status_ok && regist_status != mp::status_exist)
    {
        mp::db_manage.rollback_transaction();
        return mp::status_exec_error;
    }

    int64_t other = 0;
    //���û�д����Ʒ
    if(update_player_items(_player_id, uuid, _amount, _position, _position_weight, other) != mp::status_ok)
    {
        mp::db_manage.rollback_transaction();
        return mp::status_exec_error;
    }

    mp::db_manage.end_transaction();
    return mp::status_ok;
}

