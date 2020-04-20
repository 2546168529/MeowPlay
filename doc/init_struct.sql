/*���ע���*/
CREATE TABLE IF NOT EXISTS db_play_data.player_register(
	player_id INTEGER PRIMARY KEY AUTOINCREMENT, 	/* ���ID */
	bind_info VARCHAR								/* ����Ϣ������QQ */
);

/*��һ������Ա�*/
CREATE TABLE IF NOT EXISTS db_play_data.player_attribute(
	player_id 		INTEGER,	/* ���ID */
	attribute_name 	VARCHAR,	/* ������*/
	attribute_value VARCHAR		/* ����*/
);

/*��Ʒ����*/
CREATE TABLE db_base_data.items_info (
    item_base_id  	INTEGER PRIMARY KEY AUTOINCREMENT,	/* ��Ʒ����ID */
    item_name       VARCHAR, 							/* ��Ʒ���� */
    item_type  		INTEGER, 							/* ��Ʒ���� */
	uniqueness		INTEGER,							/* ��ƷΨһ�� */
    base_info  		VARCHAR 							/* ��Ʒ������Ϣ��Json) */
	
);

/*
** ��ƷBASE UUID
** ÿ����Ҫ�������Ե���Ʒ����ҪUUID
** ÿ���һ����Ҫ�������Ե���Ʒ�������Ʒ��uuid_base+1
** ����UUID����Ϊ��(item_base_id<<32) + uuid_base
** uuid_base��һ���޷������ͣ�ȡֵ��ΧΪ 1~4294967295, ��ֹΪ0
*/
CREATE TABLE db_play_data.items_uuid_base (
	item_base_id 	INTEGER,
	uuid_base		INTEGER
);

/*�����Ʒ*/
CREATE TABLE db_play_data.player_items (
    item_uuid  		INTEGER,	/* ��ƷUUID */
    player_id       INTEGER, 	/* ���ID */
    amount          INTEGER, 	/* ��Ʒ���� */
	position		INTEGER, 	/* ��Ʒλ�ã�����0��ʾ������1��ʾ�ֿ� */
    position_weight INTEGER,  	/* ��Ʒλ��Ȩ�أ�����������Ʒ��λ�ã��Ա���õ�չʾ����� */
	other			INTEGER		/* ������Ϣ����λ�洢 */
);


/*��Ʒע��*/
CREATE TABLE db_play_data.items_register (
    item_uuid  		INTEGER, 	/* ��ƷUUID��ͨ�������õ���ΨһID�������Ʒ�ǿɵ������ͣ���ôitem_uuid==item_base_id */
    item_base_id 	INTEGER,	/* ��Ʒ����ID */
	regist_time 	DATETIME,	/* ��Ʒע��ʱ�� */
	time_limit 		INTEGER,	/* ��Ʒ���� */
	weight 			INTEGER		/* ��Ʒ���� */
);

/*��Ʒ���Ա�*/
CREATE TABLE db_play_data.items_attributes (
    item_uuid  			INTEGER, 	/* ��ƷUUID */
	attributes_name 	VARCHAR, 	/* �������ƣ������������;�... */
	attributes_value 	VARCHAR 	/* ��Ӧ������ֵ */
);

