#include "items_system.h"
#include <inttypes.h>
#include "../json/json11.hpp"

/**
** ��ȡ��Ʒ������Ϣ
** @param _item_base_id ��Ʒ����ID
** @param _read_result ��ȡ���
** @return �Ƿ��ȡ�ɹ�
*/
bool read_base_items(uint32_t _item_base_id, mp::BaseItemsInfo& _read_result)
{
    /* ������ǰ״̬Ϊ��ֹ�����ݿ⣬ȡ������ */
	if (mp::app_info.status_ban_read_database)
		return false;

    bool result_flag = false;
    mp::database::stmt query_stmt = mp::db_manage.prepare(
        "SELECT item_name,item_type,uniqueness,base_info FROM db_base_data.items_info WHERE item_base_id=@item_base_id",
        {"@item_base_id"}, _item_base_id);


    if(query_stmt.step() == SQLITE_ROW)
    {
        result_flag = true;
        
        _read_result.item_base_id = _item_base_id;
        query_stmt.column(0, _read_result.item_name);
        query_stmt.column(1, _read_result.item_type);
        _read_result.uniqueness = (query_stmt.column<int32_t>(2) != 0);
        query_stmt.column(3, _read_result.base_info);

    }

    return result_flag;
}

/**
** ���������Ʒ��Ϣ
** @param _player_id ���ID
** @param _item_uuid ��ƷUUID��ȫ��ΨһID��
** @param _amount ��Ʒ��������Ϊ����
** @param _position ��Ʒλ�ô���
** @param _position_weight ��Ʒλ��Ȩ�أ���������
** @param _other ��Ʒ�������ԣ��ӵ͵���λ����һλ��ʾ��Ʒ�Ƿ��������ڶ�λ��ʾ����Ʒ�Ƿ�������ҽ���
** @return �Ƿ�ɹ����������Ʒ������ʧ�ܵ�����У���ֹ��������дʧ�ܡ���Ʒ����-_amount<0
*/
bool update_player_items(int64_t _player_id, int64_t _item_uuid, int32_t _amount, int32_t _position, int32_t _position_weight, int64_t _other)
{
    bool result_flag = false;

    /* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return false;
	/* ������ǰ״̬Ϊ��ֹд���ݿ⣬ȡ������ */
	if (mp::app_info.status_ban_write_database)
		return false;

	mp::lock_write.lock();

    mp::database::stmt query_stmt = mp::db_manage.prepare(
        "SELECT amount FROM db_play_data.player_items WHERE item_uuid=@item_uuid AND _player_id=@player_id",
        {"@item_uuid", "@player_id"},
        _item_uuid, _player_id);

    
    if(query_stmt.step() == SQLITE_ROW)
    {
        //����
        int64_t final_amount = query_stmt.column<int64_t>(0) + _amount;
        if(final_amount > 0)
        {
            //���������������0���޸�����Ϊ��������
            result_flag = mp::db_manage.exec_noquery(
                "UPDATE db_play_data.player_items SET amount=@amount WHERE item_uuid=@item_uuid,position=@position,position_weight=@position_weight,other=@other AND _player_id=@player_id",
                {"@item_uuid", "@player_id", "@amount", "@position", "@position_weight", "@other"},
                _item_uuid, _player_id, final_amount, _position, _position_weight, _other);
        }
        else if(final_amount == 0)
        {
            //��������������0��ֱ��ɾ��
            result_flag = mp::db_manage.exec_noquery(
                "DELETE FROM db_play_data.player_items WHERE item_uuid=@item_uuid AND _player_id=@player_id",
                {"@item_uuid", "@player_id"},
                _item_uuid, _player_id);
        }
        else
        {
            //��������С��0
            result_flag = false;
        }
        
    }
    else
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
    
    mp::lock_write.unlock();
    return result_flag;
}


bool add_player_items(int64_t _player_id, uint32_t _item_base_id, int64_t _amount, int32_t _position, int32_t _position_weight, int64_t time_limit)
{
    bool result_flag = false;

    /* �����������״̬��ȡ������ */
	if (!mp::app_info.status_runtime)
		return false;
	/* ������ǰ״̬Ϊ��ֹд���ݿ⣬ȡ������ */
	if (mp::app_info.status_ban_write_database)
		return false;

	mp::lock_write.lock();

    mp::BaseItemsInfo baseItemsInfo;
    if(read_base_items(_item_base_id, baseItemsInfo))
    {

    }

    mp::lock_write.unlock();
    return result_flag;
}

