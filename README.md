<!-- README.md --- 
;; 
;; Description: 
;; Author: Hongyi Wu(吴鸿毅)
;; Email: wuhongyi@qq.com 
;; Created: 三 5月 22 10:17:58 2019 (+0800)
;; Last-Updated: 五 6月 28 16:23:24 2019 (+0800)
;;           By: Hongyi Wu(吴鸿毅)
;;     Update #: 4
;; URL: http://wuhongyi.cn -->

# README

> https://www.runoob.com/sql/sql-tutorial.html

## C++

> https://www.cnblogs.com/wunaozai/p/3618383.html  
> http://www.cnblogs.com/cobbliu/p/3709051.html  


## Python

> https://www.cnblogs.com/lxt287994374/p/3910509.html  


### 引入MySQLdb库

```python
import MySQLdb
```
 

### 和数据库建立连接

```python
conn=MySQLdb.connect(host="localhost",user="root",passwd="sa",db="mytable",charset="utf8")
```

提供的 connect 方法用来和数据库建立连接,接收数个参数,返回连接对象.

比较常用的参数包括
- host:数据库主机名.默认是用本地主机.
- user:数据库登陆名.默认是当前用户.
- passwd:数据库登陆的秘密.默认为空.
- db:要使用的数据库名.没有默认值.
- port:MySQL服务使用的TCP端口.默认是3306.
- charset:数据库编码.

然后,这个连接对象也提供了对事务操作的支持,标准的方法

```
commit() 提交
rollback() 回滚
```
 

### 执行sql语句和接收返回值

```python
cursor=conn.cursor()
n=cursor.execute(sql,param)
```

首先,我们用使用连接对象获得一个cursor对象,接下来,我们会使用cursor提供的方法来进行工作.这些方法包括两大类:1.执行命令,2.接收返回值

 
cursor用来执行命令的方法:
```
callproc(self, procname, args):用来执行存储过程,接收的参数为存储过程名和参数列表,返回值为受影响的行数
execute(self, query, args):执行单条sql语句,接收的参数为sql语句本身和使用的参数列表,返回值为受影响的行数
executemany(self, query, args):执行单条sql语句,但是重复执行参数列表里的参数,返回值为受影响的行数
nextset(self):移动到下一个结果集
```
 

cursor用来接收返回值的方法:
```
fetchall(self):接收全部的返回结果行.
fetchmany(self, size=None):接收size条返回结果行.如果size的值大于返回的结果行的数量,则会返回cursor.arraysize条数据.
fetchone(self):返回一条结果行.
scroll(self, value, mode='relative'):移动指针到某一行.如果mode='relative',则表示从当前所在行移动value条,如果mode='absolute',则表示从结果集的第一行移动value条.
```
 

下面的代码是一个完整的例子.
```python
#使用sql语句,这里要接收的参数都用%s占位符.要注意的是,无论你要插入的数据是什么类型,占位符永远都要用%s
sql="insert into cdinfo values(%s,%s,%s,%s,%s)"

#param应该为tuple或者list
param=(title,singer,imgurl,url,alpha)

#执行,如果成功,n的值为1
n=cursor.execute(sql,param)


#再来执行一个查询的操作
cursor.execute("select * from cdinfo")

#我们使用了fetchall这个方法.这样,cds里保存的将会是查询返回的全部结果.每条结果都是一个tuple类型的数据,这些tuple组成了一个tuple
cds=cursor.fetchall()

#因为是tuple,所以可以这样使用结果集
print cds[0][3]

#或者直接显示出来,看看结果集的真实样子
print cds

#如果需要批量的插入数据,就这样做
sql="insert into cdinfo values(0,%s,%s,%s,%s,%s)"

#每个值的集合为一个tuple,整个参数集组成一个tuple,或者list
param=((title,singer,imgurl,url,alpha),(title2,singer2,imgurl2,url2,alpha2))

#使用executemany方法来批量的插入数据.这真是一个很酷的方法!
n=cursor.executemany(sql,param)
```
 

### 关闭数据库连接

需要分别的关闭指针对象和连接对象.他们有名字相同的方法
```python
cursor.close()
conn.close()
```

<!-- README.md ends here -->
