#include "items_system.h"
#include <inttypes.h>
#include "../json/json11.hpp"

/**
** 读取物品基础信息
** @param _item_base_id 物品基础ID
** @param _read_result 读取结果
** @return 是否读取成功
*/
bool read_base_items(uint32_t _item_base_id, mp::BaseItemsInfo& _read_result)
{
    /* 若程序当前状态为禁止读数据库，取消操作 */
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
** 更新玩家物品信息
** @param _player_id 玩家ID
** @param _item_uuid 物品UUID（全局唯一ID）
** @param _amount 物品数量，可为负数
** @param _position 物品位置代码
** @param _position_weight 物品位置权重，用于排序
** @param _other 物品其它属性，从低到高位，第一位表示物品是否锁定，第二位表示该物品是否允许玩家解锁
** @return 是否成功更新玩家物品，更新失败的情况有：禁止操作、读写失败、物品数量-_amount<0
*/
bool update_player_items(int64_t _player_id, int64_t _item_uuid, int32_t _amount, int32_t _position, int32_t _position_weight, int64_t _other)
{
    bool result_flag = false;

    /* 若程序非运行状态，取消操作 */
	if (!mp::app_info.status_runtime)
		return false;
	/* 若程序当前状态为禁止写数据库，取消操作 */
	if (mp::app_info.status_ban_write_database)
		return false;

	mp::lock_write.lock();

    mp::database::stmt query_stmt = mp::db_manage.prepare(
        "SELECT amount FROM db_play_data.player_items WHERE item_uuid=@item_uuid AND _player_id=@player_id",
        {"@item_uuid", "@player_id"},
        _item_uuid, _player_id);

    
    if(query_stmt.step() == SQLITE_ROW)
    {
        //存在
        int64_t final_amount = query_stmt.column<int64_t>(0) + _amount;
        if(final_amount > 0)
        {
            //最终数量如果大于0，修改数量为最终数量
            result_flag = mp::db_manage.exec_noquery(
                "UPDATE db_play_data.player_items SET amount=@amount WHERE item_uuid=@item_uuid,position=@position,position_weight=@position_weight,other=@other AND _player_id=@player_id",
                {"@item_uuid", "@player_id", "@amount", "@position", "@position_weight", "@other"},
                _item_uuid, _player_id, final_amount, _position, _position_weight, _other);
        }
        else if(final_amount == 0)
        {
            //最终数量若等于0，直接删除
            result_flag = mp::db_manage.exec_noquery(
                "DELETE FROM db_play_data.player_items WHERE item_uuid=@item_uuid AND _player_id=@player_id",
                {"@item_uuid", "@player_id"},
                _item_uuid, _player_id);
        }
        else
        {
            //最终数量小于0
            result_flag = false;
        }
        
    }
    else
    {
        //玩家物品表内不存在这个物品
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

    /* 若程序非运行状态，取消操作 */
	if (!mp::app_info.status_runtime)
		return false;
	/* 若程序当前状态为禁止写数据库，取消操作 */
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

