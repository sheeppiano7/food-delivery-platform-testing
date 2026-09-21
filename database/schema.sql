PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS userinof (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    userId VARCHAR(40) NOT NULL,
    userName VARCHAR(40) NOT NULL,
    account INTEGER UNIQUE,
    password INTEGER NOT NULL,
    phoneNum INTEGER NOT NULL,
    address VARCHAR(40) NOT NULL,
    usertype INTEGER NOT NULL,
    onsalelevel INTEGER,
    coupon INTEGER,
    managernum INTEGER
);

CREATE TABLE IF NOT EXISTS custommenu (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    menuidx INTEGER NOT NULL,
    name VARCHAR(40) NOT NULL,
    price REAL NOT NULL,
    discribe VARCHAR(100) NOT NULL,
    imgurl VARCHAR(100) NOT NULL,
    uuid VARCHAR(40) NOT NULL,
    merchantname VARCHAR(40) NOT NULL
);

CREATE TABLE IF NOT EXISTS menuorder (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    orderid INTEGER NOT NULL,
    orderuuid VARCHAR(40) NOT NULL,
    customerphonenum INTEGER NOT NULL,
    customeraddress VARCHAR(100) NOT NULL,
    merchantaddress VARCHAR(40) NOT NULL,
    customeruuid VARCHAR(40) NOT NULL,
    price REAL NOT NULL,
    orderstatu INTEGER NOT NULL,
    name VARCHAR(40) NOT NULL,
    orderdate VARCHAR(40) NOT NULL,
    merchantname VARCHAR(40) NOT NULL
);
