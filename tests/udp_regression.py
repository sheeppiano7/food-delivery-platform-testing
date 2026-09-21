from pathlib import Path
import os, shutil, subprocess, socket, struct, sqlite3, time, json, hashlib, datetime, tempfile

exe_value=os.environ.get('TAKEAWAY_SERVER_EXE')
if not exe_value:
    raise RuntimeError('Set TAKEAWAY_SERVER_EXE to the locally built Qt server executable')
exe=Path(exe_value).resolve()
if not exe.is_file():
    raise FileNotFoundError(exe)
OUT=Path(os.environ.get('TAKEAWAY_TEST_OUT', Path(tempfile.gettempdir())/'takeaway-protocol-tests')).resolve()
OUT.mkdir(parents=True,exist_ok=True)
RUN=OUT/datetime.datetime.now().strftime('run_%H%M%S')
RUN.mkdir();(RUN/'config').mkdir()
shutil.copy2(exe,RUN/'TakeawayPlatformService.exe')
env=os.environ.copy()
qt_bin=os.environ.get('QT_RUNTIME_BIN','')
if qt_bin: env['PATH']=qt_bin+os.pathsep+env['PATH']
env['QT_QPA_PLATFORM']='offscreen'
sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);sock.bind(('127.0.0.1',0));sock.settimeout(.2)
probe=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);probe.bind(('127.0.0.1',0));server_port=probe.getsockname()[1];probe.close()
(RUN/'config/NetworkConfig.xml').write_text(f'<Config><IpConfig pathkey="test"><local ip="127.0.0.1" port="{server_port}"/><target ip="127.0.0.1" port="{sock.getsockname()[1]}"/></IpConfig></Config>',encoding='utf8')
original_dbs=[]
hashes={}
log=open(RUN/'service.log','w',encoding='utf8')
p=subprocess.Popen([str(RUN/'TakeawayPlatformService.exe')],cwd=RUN,env=env,stdout=log,stderr=log,creationflags=subprocess.CREATE_NO_WINDOW)
db=RUN/'CustomMenuSys.db';rows=[]
def snap():
    with sqlite3.connect(f'file:{db.as_posix()}?mode=ro',uri=True) as c:
        return {t:c.execute('select * from '+t).fetchall() for t in ['userinof','custommenu','menuorder']}
def send(data):sock.sendto(data,('127.0.0.1',server_port));time.sleep(.35)
fmt='<40sH40s40sQ50s50sB40s40sd'
def order(uid='QA_ORDER_1',status=0,price=25.5,name='QA_FOOD',customer='QA_CUSTOMER'):
    b=lambda s:s.encode('utf8')
    return struct.pack(fmt,b(uid),1,b(name),b('QA_SHOP'),13900000001,b('QA_ADDRESS'),b('QA_SHOP_ADDRESS'),status,b(customer),b('2026-09-16'),price)
def batch(items):return struct.pack('<HB',0xaaff,len(items))+b''.join(items)+bytes(struct.calcsize(fmt)*(200-len(items)))
userfmt='<40s40sQQQ100sBIIQ'
menufmt='<H40sd50s50s40s40s'
def user(uid,account,name='QA_USER',role=11):
    enc=lambda s:s.encode('utf8')
    return struct.pack('<H',0xaacc)+struct.pack(userfmt,enc(uid),enc(name),account,123456,13900000001,enc('QA_ADDRESS'),role,0,0,0)
def menu(uid='QA_MENU_A',shop='QA_SHOP',price=25.5,name='QA_FOOD'):
    enc=lambda s:s.encode('utf8')
    return struct.pack(menufmt,1,enc(name),price,enc('QA_DESCRIPTION'),enc(''),enc(uid),enc(shop))
def menus(items):return struct.pack('<HB',0xaaee,len(items))+b''.join(items)+bytes(struct.calcsize(menufmt)*(200-len(items)))
def case(id,title,data,check,expected,defect=''):
    before=snap();start=datetime.datetime.now().isoformat(timespec='seconds');send(data);after=snap();ok=check(after,before)
    rows.append(dict(id=id,title=title,expected=expected,status='通过' if ok else '失败',defect=defect if not ok else '',time=start,before=before,after=after))
try:
    for _ in range(40):
        if p.poll() is not None:raise RuntimeError('service exit '+str(p.returncode))
        if db.exists():
            try:
                initial=snap();break
            except sqlite3.Error:pass
        time.sleep(.1)
    else:raise RuntimeError('database not ready')
    ready=False
    for _ in range(30):
        try:
            packet,_=sock.recvfrom(65535)
            if len(packet)>=2 and struct.unpack_from('<H',packet)[0]==0x1122:ready=True;break
        except socket.timeout:pass
    if not ready:raise RuntimeError('UDP heartbeat not ready')
    for i,(role,name) in enumerate([(10,'QA_SHOP'),(11,'QA_CUSTOMER'),(12,'QA_RIDER'),(13,'QA_ADMIN')]):
        case('REG-00'+str(i+1),'注册角色'+name,user(name,910001+i,name,role),lambda a,b,i=i,role=role:len(a['userinof'])==len(b['userinof'])+1 and any(x[3]==910001+i and x[7]==role for x in a['userinof']),'增加1条账号，账号和角色数值准确')
    case('REG-005','重复账号',user('QA_DUP_USER',910001),lambda a,b:a==b,'拒绝重复账号，数据不变')
    case('REG-006','空用户名',user('QA_EMPTY_USER',910006,''),lambda a,b:a==b,'拒绝空用户名','TAKE-006')
    case('REG-007','单引号用户名',user('QA_QUOTE_USER',910007,"O'Neil"),lambda a,b:any(x[3]==910007 for x in a['userinof']),'合法姓名可以保存','TAKE-004')
    case('REG-008','非法角色255',user('QA_BAD_ROLE',910008,role=255),lambda a,b:a==b,'拒绝非法角色','TAKE-006')
    def get_users():
        sock.sendto(struct.pack('<H',0xaabb),('127.0.0.1',server_port))
        end=time.monotonic()+3
        while time.monotonic()<end:
            try:data,_=sock.recvfrom(65535)
            except socket.timeout:continue
            if len(data)==3+100*struct.calcsize(userfmt) and struct.unpack_from('<H',data)[0]==0xaabb:
                return [struct.unpack_from(userfmt,data,3+i*struct.calcsize(userfmt)) for i in range(data[2])]
        raise RuntimeError('user list response timeout')
    before=snap();reported=get_users()
    rows.append(dict(id='DATA-001',title='手机号从数据库到UDP响应一致性',expected='返回原始11位手机号13900000001',status='通过' if all(x[4]==13900000001 for x in reported) and reported else '失败',defect='TAKE-008',time=datetime.datetime.now().isoformat(timespec='seconds'),before=before,after=snap(),actual={'returned_phones':[x[4] for x in reported]}))
    rows.append(dict(id='AUTH-002',title='未登录读取账号列表的密码字段',expected='未认证请求不返回可用明文密码',status='通过' if not any(x[3]==123456 for x in reported) else '失败',defect='TAKE-009',time=datetime.datetime.now().isoformat(timespec='seconds'),before=before,after=snap(),actual={'synthetic_password_exposed':any(x[3]==123456 for x in reported),'user_count':len(reported)}))
    case('MENU-001','新增菜品价格25.5',menus([menu()]),lambda a,b:len(a['custommenu'])==1 and a['custommenu'][0][3]==25.5,'新增菜品，金额准确')
    case('MENU-002','菜品编辑价格',menus([menu(price=26.75)]),lambda a,b:len(a['custommenu'])==1 and a['custommenu'][0][3]==26.75,'同商户菜单替换，价格26.75')
    case('MENU-003','第二商户菜单不覆盖第一商户',menus([menu(uid='QA_MENU_B',shop='QA_SHOP_B')]),lambda a,b:len(a['custommenu'])==2,'保留第一商户菜品，新增第二商户菜品')
    case('MENU-004','重复提交同商户菜单',menus([menu(price=26.75)]),lambda a,b:len(a['custommenu'])==2,'不产生重复菜品')
    case('MENU-005','负菜品价格',menus([menu(price=-3)]),lambda a,b:a==b,'负价格拒绝且保留旧菜单','TAKE-002')
    case('MENU-006','单引号菜品名更新',menus([menu(name="Chef's rice")]),lambda a,b:any(x[6]=='QA_MENU_A' for x in a['custommenu']),'合法名称可以更新，异常也不得删除原菜单','TAKE-004')
    case('MENU-007','恢复合成菜单',menus([menu()]),lambda a,b:any(x[6]=='QA_MENU_A' and x[3]==25.5 for x in a['custommenu']),'恢复合成菜单用于后续订单测试')
    case('MENU-008','同批重复菜品UUID',menus([menu(),menu()]),lambda a,b:sum(x[6]=='QA_MENU_A' for x in a['custommenu'])==1,'同一UUID只保存1条菜品','TAKE-003')
    case('MENU-009','混合商户报文',menus([menu(),menu(uid='QA_MENU_B',shop='QA_SHOP_B')]),lambda a,b:sum(x[6]=='QA_MENU_B' for x in a['custommenu'])==1,'混合批次不得制造重复菜品','TAKE-003')
    count=lambda a:len(a['menuorder'])
    case('ORD-001','合法订单写入与金额字段保存',batch([order()]),lambda a,b:count(a)==1 and a['menuorder'][0][7]==25.5,'存储1条订单，金额25.5')
    for n in [1,2,3]:case('ORD-00'+str(n+1),'订单正常状态更新 '+str(n),batch([order(status=n)]),lambda a,b,n=n:count(a)==1 and a['menuorder'][0][8]==n,'仍1条订单，状态更新为'+str(n))
    case('ORD-005','单条报文重复提交',batch([order(status=3)]),lambda a,b:count(a)==1,'相同订单重复发送不新增第二条')
    case('ORD-006','已完成订单回退待接单',batch([order(status=0)]),lambda a,b:a['menuorder'][0][8]==3,'已完成订单不得回退到新建状态','TAKE-001')
    case('ORD-007','非法枚举状态255',batch([order(status=255)]),lambda a,b:a==b,'拒绝非法状态，数据不变化','TAKE-001')
    case('ORD-008','负订单金额',batch([order(uid='QA_NEG',price=-1)]),lambda a,b:a==b,'拒绝负金额，数据不变化','TAKE-002')
    case('ORD-009','同批两条重复订单',batch([order(uid='QA_DUP'),order(uid='QA_DUP')]),lambda a,b:sum(r[2]=='QA_DUP' for r in a['menuorder'])==1,'同一业务订单只能保存1条','TAKE-003')
    case('ORD-010','合法名称包含单引号',batch([order(uid='QA_QUOTE',name="Chef's rice")]),lambda a,b:any(r[2]=='QA_QUOTE' and r[9]=="Chef's rice" for r in a['menuorder']),'合法菜品名称保留单引号并正常保存','TAKE-004')
    case('ORD-011','已存在订单更新为含单引号名称',batch([order(name="Chef's rice")]),lambda a,b:any(r[2]=='QA_ORDER_1' for r in a['menuorder']),'更新失败时也不得丢失原订单','TAKE-004')
    case('NET-001','短报文',b'\xff',lambda a,b:a==b,'忽略短报文，数据库不变')
    case('NET-002','未知消息类型',struct.pack('<H',0x9999),lambda a,b:a==b,'忽略未知报文，数据库不变')
    case('NET-003','订单报文长度不足',batch([order()])[:-1],lambda a,b:a==b,'拒绝不完整报文')
    case('NET-004','订单报文多出1字节',batch([order()])+b'\x00',lambda a,b:a==b,'拒绝长度不符报文')
    case('ORD-012','六位整数金额的小数精度',batch([order(uid='QA_PRECISION',price=123456.78)]),lambda a,b:any(x[2]=='QA_PRECISION' and abs(x[7]-123456.78)<.005 for x in a['menuorder']),'123456.78金额精确保存','TAKE-007')
    # Endpoint has no session/login handshake. Explicitly use an unconfigured sender.
    before=snap();alien=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);alien.sendto(batch([order(uid='QA_UNAUTH')]),('127.0.0.1',server_port));alien.close();time.sleep(.35);after=snap()
    rows.append(dict(id='AUTH-001',title='未配置客户端未登录提交订单',expected='未认证来源不能创建订单',status='通过' if before==after else '失败',defect='' if before==after else 'TAKE-005',time=datetime.datetime.now().isoformat(timespec='seconds'),before=before,after=after))
    case('REG-009','中文用户名原样保存',user('QA_CN_USER',910009,'测试用户'),lambda a,b:any(x[3]==910009 and x[2]=='测试用户' for x in a['userinof']),'中文姓名完整保存')
    case('ORD-013','中文菜品名称与小数金额',batch([order(uid='QA_CN_ORDER',name='测试炒饭',price=.01)]),lambda a,b:any(x[2]=='QA_CN_ORDER' and x[9]=='测试炒饭' and x[7]==.01 for x in a['menuorder']),'中文名称和0.01金额正确保存')
    case('ORD-014','空订单UUID',batch([order(uid='')]),lambda a,b:a==b,'拒绝空业务标识','TAKE-010')
    case('ORD-015','空读者对应的客户标识',batch([order(uid='QA_EMPTY_CUSTOMER',customer='')]),lambda a,b:a==b,'订单必须关联客户，拒绝空客户标识','TAKE-010')
    case('ORD-016','空菜品名称',batch([order(uid='QA_EMPTY_NAME',name='')]),lambda a,b:a==b,'拒绝缺少菜品内容的订单','TAKE-010')
    case('ORD-017','常规金额9999.99',batch([order(uid='QA_PRICE_NORMAL',price=9999.99)]),lambda a,b:any(x[2]=='QA_PRICE_NORMAL' and x[7]==9999.99 for x in a['menuorder']),'9999.99原样保存')
    case('NET-005','零字节数据报',b'',lambda a,b:a==b,'忽略空数据报，状态不变')
    case('NET-006','仅订单消息头',struct.pack('<H',0xaaff),lambda a,b:a==b,'拒绝只有消息头的报文')
    case('NET-007','空订单批次',batch([]),lambda a,b:a==b,'空批次不删除或新增订单')
    case('NET-008','空菜品批次',menus([]),lambda a,b:a==b,'空批次无法指定商户，不得删除其他菜单')
    before=snap();reported=get_users()
    rows.append(dict(id='DATA-002',title='角色账号与中文姓名返回一致性',expected='返回角色10至13，账号和中文用户名与数据库一致',status='通过' if all(any(x[2]==910001+i and x[6]==10+i for x in reported) for i in range(4)) and any(x[2]==910009 and x[1].split(b'\x00')[0].decode('utf8')=='测试用户' for x in reported) else '失败',defect='',time=datetime.datetime.now().isoformat(timespec='seconds'),before=before,after=snap()))
    # Targeted follow-up with synthetic failure injection in this new database only.
    case('FIX-MENU-PRICE','菜品金额123456.78',menus([menu(price=123456.78)]),lambda a,b:any(x[6]=='QA_MENU_A' and abs(x[3]-123456.78)<.0001 for x in a['custommenu']),'菜品价格精确保留小数','TAKE-007')
    case('FIX-MENU-QUOTE','单引号菜品完整保存',menus([menu(name="Chef's rice")]),lambda a,b:any(x[6]=='QA_MENU_A' and x[2]=="Chef's rice" for x in a['custommenu']),'菜品名称原样保存','TAKE-004')
    case('FIX-ORDER-SEED','准备事务回滚订单',batch([order(uid='QA_ROLLBACK',name='SAFE_ORIGINAL')]),lambda a,b:any(x[2]=='QA_ROLLBACK' and x[9]=='SAFE_ORIGINAL' for x in a['menuorder']),'创建专用合成订单')
    with sqlite3.connect(db) as con:
        con.execute("CREATE TRIGGER qa_abort_order BEFORE INSERT ON menuorder WHEN NEW.name = 'QA_FAIL_INSERT' BEGIN SELECT RAISE(ABORT, 'synthetic insert failure'); END")
    case('FIX-ORDER-ROLLBACK','订单删除后插入失败回滚',batch([order(uid='QA_ROLLBACK',name='QA_FAIL_INSERT')]),lambda a,b:a==b,'强制插入失败，完整保留原订单','TAKE-004')
    with sqlite3.connect(db) as con:
        con.execute('DROP TRIGGER qa_abort_order')
        con.execute("CREATE TRIGGER qa_abort_menu BEFORE INSERT ON custommenu WHEN NEW.name = 'QA_FAIL_INSERT' BEGIN SELECT RAISE(ABORT, 'synthetic insert failure'); END")
    case('FIX-MENU-ROLLBACK','整批菜品删除后插入失败回滚',menus([menu(name='QA_FAIL_INSERT')]),lambda a,b:a==b,'强制插入失败，完整保留原菜单','TAKE-004')
    with sqlite3.connect(db) as con:con.execute('DROP TRIGGER qa_abort_menu')
    case('FIX-RECOVERY','失败事务后继续更新订单',batch([order(uid='QA_ROLLBACK',name="Chef's rice",price=123456.78)]),lambda a,b:any(x[2]=='QA_ROLLBACK' and x[9]=="Chef's rice" and abs(x[7]-123456.78)<.0001 for x in a['menuorder']),'失败后服务仍可用，单引号和金额准确','TAKE-004')
    before=snap();stamp=datetime.datetime.now().isoformat(timespec='seconds');p.terminate();p.wait(timeout=5)
    p=subprocess.Popen([str(RUN/'TakeawayPlatformService.exe')],cwd=RUN,env=env,stdout=log,stderr=log,creationflags=subprocess.CREATE_NO_WINDOW)
    time.sleep(1)
    restored=get_users();after=snap()
    rows.append(dict(id='ENV-001',title='独立服务重启后的数据持久性',expected='重启后表数据不丢失且用户列表可查询',status='通过' if before==after and restored and p.poll() is None else '失败',defect='',time=stamp,before=before,after=after))
finally:
    if p.poll() is None:p.terminate();p.wait(timeout=5)
    log.close();sock.close()
    unchanged={str(f):hashlib.sha256(f.read_bytes()).hexdigest()==hashes[str(f)] for f in original_dbs}
    report=dict(mode='真实Qt服务端UDP协议层测试，非客户端界面测试',run=str(RUN),exe_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),original_db_unchanged=unchanged,cases=rows)
    (RUN/'results.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf8')
    print(json.dumps({'run':str(RUN),'counts':{s:sum(r['status']==s for r in rows) for s in ['通过','失败']},'unchanged':unchanged},ensure_ascii=False))
