BEGIN TRANSACTION;
DELETE FROM menuorder WHERE orderuuid LIKE 'QA_PUBLIC_%';
DELETE FROM custommenu WHERE uuid LIKE 'QA_PUBLIC_%';
DELETE FROM userinof WHERE userId LIKE 'QA_PUBLIC_%';

INSERT INTO userinof
(userId, userName, account, password, phoneNum, address, usertype, onsalelevel, coupon, managernum)
VALUES
('QA_PUBLIC_CUSTOMER', '测试用户', 900001, 100001, 10000000001, '测试地址A', 0, 0, 1, 0),
('QA_PUBLIC_MERCHANT', '测试商家', 900002, 100002, 10000000002, '测试地址B', 1, 1, 0, 0),
('QA_PUBLIC_RIDER', '测试骑手', 900003, 100003, 10000000003, '测试地址C', 2, 0, 0, 0),
('QA_PUBLIC_ADMIN', '测试管理员', 900004, 100004, 10000000004, '测试地址D', 3, 0, 0, 1);

INSERT INTO custommenu
(menuidx, name, price, discribe, imgurl, uuid, merchantname)
VALUES
(1, '测试套餐A', 12.34, '合成测试菜品', 'qa://image/a', 'QA_PUBLIC_MENU_A', '测试商家'),
(2, 'O''Brien套餐', 123456.78, '单引号与金额精度回归', 'qa://image/b', 'QA_PUBLIC_MENU_B', '测试商家');

INSERT INTO menuorder
(orderid, orderuuid, customerphonenum, customeraddress, merchantaddress, customeruuid, price, orderstatu, name, orderdate, merchantname)
VALUES
(1, 'QA_PUBLIC_ORDER_001', 10000000001, '测试地址A', '测试地址B', 'QA_PUBLIC_CUSTOMER', 12.34, 0, '测试套餐A', '2026-09-16 20:00:00', '测试商家');
COMMIT;
