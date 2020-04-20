/*玩家注册绑定*/
CREATE TABLE IF NOT EXISTS db_play_data.player_register(
	player_id INTEGER PRIMARY KEY AUTOINCREMENT, 	/* 玩家ID */
	bind_info VARCHAR								/* 绑定信息，例如QQ */
);

/*玩家基础属性表*/
CREATE TABLE IF NOT EXISTS db_play_data.player_attribute(
	player_id 		INTEGER,	/* 玩家ID */
	attribute_name 	VARCHAR,	/* 属性名*/
	attribute_value VARCHAR		/* 属性*/
);

/*物品基础*/
CREATE TABLE db_base_data.items_info (
    item_base_id  	INTEGER PRIMARY KEY AUTOINCREMENT,	/* 物品基础ID */
    item_name       VARCHAR, 							/* 物品名称 */
    item_type  		INTEGER, 							/* 物品类型 */
	uniqueness		INTEGER,							/* 物品唯一性 */
    base_info  		VARCHAR 							/* 物品基础信息（Json) */
	
);

/*
** 物品BASE UUID
** 每个需要独立属性的物品都需要UUID
** 每获得一个需要独立属性的物品，这个物品的uuid_base+1
** 最终UUID计算为：(item_base_id<<32) + uuid_base
** uuid_base是一个无符号整型，取值范围为 1~4294967295, 禁止为0
*/
CREATE TABLE db_play_data.items_uuid_base (
	item_base_id 	INTEGER,
	uuid_base		INTEGER
);

/*玩家物品*/
CREATE TABLE db_play_data.player_items (
    item_uuid  		INTEGER,	/* 物品UUID */
    player_id       INTEGER, 	/* 玩家ID */
    amount          INTEGER, 	/* 物品数量 */
	position		INTEGER, 	/* 物品位置，例如0表示背包，1表示仓库 */
    position_weight INTEGER,  	/* 物品位置权重，用于排序物品的位置，以便更好的展示给玩家 */
	other			INTEGER		/* 其它信息，按位存储 */
);


/*物品注册*/
CREATE TABLE db_play_data.items_register (
    item_uuid  		INTEGER, 	/* 物品UUID，通过计算获得到的唯一ID，如果物品是可叠加类型，那么item_uuid==item_base_id */
    item_base_id 	INTEGER,	/* 物品基础ID */
	regist_time 	DATETIME,	/* 物品注册时间 */
	time_limit 		INTEGER,	/* 物品期限 */
	weight 			INTEGER		/* 物品重量 */
);

/*物品属性表*/
CREATE TABLE db_play_data.items_attributes (
    item_uuid  			INTEGER, 	/* 物品UUID */
	attributes_name 	VARCHAR, 	/* 属性名称：例如生命，耐久... */
	attributes_value 	VARCHAR 	/* 对应的属性值 */
);

