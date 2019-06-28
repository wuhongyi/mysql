<!-- rootsql.md --- 
;; 
;; Description: 
;; Author: Hongyi Wu(吴鸿毅)
;; Email: wuhongyi@qq.com 
;; Created: 五 6月 28 13:06:22 2019 (+0800)
;; Last-Updated: 五 6月 28 14:36:03 2019 (+0800)
;;           By: Hongyi Wu(吴鸿毅)
;;     Update #: 3
;; URL: http://wuhongyi.cn -->

# rootsql

## createdb

file **runcatalog.sql** 记得最后空两行
```
CREATE TABLE runcatalog
(
   dataset       VARCHAR(32) NOT NULL,
   run           INT         NOT NULL,
   firstevent    INT,
   events        INT,
   tag           INT,
   energy        FLOAT,
   runtype       ENUM('physics','cosmics','test'),
   target        VARCHAR(10),
   timef         TIMESTAMP NOT NULL,
   timel         TIMESTAMP NOT NULL,
   rawfilepath   VARCHAR(128),
   comments      VARCHAR(80)
);


```


```cpp
void sqlcreatedb()
{
   // read in runcatalog table definition
   FILE *fp = fopen("runcatalog.sql", "r");
   char sql[4096];
   fread(sql, 1, 4096, fp);
   fclose(fp);

   // open connection to MySQL server on localhost
   TSQLServer *db = TSQLServer::Connect("mysql://localhost/test", "admin", "123456");

   TSQLResult *res;

   // create new table (delete old one first if exists)
   res = db->Query("DROP TABLE IF EXISTS runcatalog");
   delete res;

   res = db->Query(sql);
   delete res;

   delete db;
}
```

```python
from ROOT import TSQLServer


# read in runcatalog table definition
fp = open("runcatalog.sql", "r")
sql = fp.read()
fp.close()

# open connection to MySQL server on localhost
db = TSQLServer.Connect("mysql://localhost/test", "admin", "123456")

# create new table (delete old one first if exists)
res = db.Query("DROP TABLE IF EXISTS runcatalog")

res = db.Query(sql)
```

## select

```cpp
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>


void sqlselect()
{
   TSQLServer *db = TSQLServer::Connect("mysql://localhost/test","admin", "123456");

   printf("Server info: %s\n", db->ServerInfo());

   TSQLRow *row;
   TSQLResult *res;

   // list databases available on server
   printf("\nList all databases on server %s\n", db->GetHost());
   res = db->GetDataBases();
   while ((row = res->Next())) {
      printf("%s\n", row->GetField(0));
      delete row;
   }
   delete res;

   // list tables in database "test" (the permission tables)
   printf("\nList all tables in database \"test\" on server %s\n",
          db->GetHost());
   res = db->GetTables("test");
   while ((row = res->Next())) {
      printf("%s\n", row->GetField(0));
      delete row;
   }
   delete res;

   // list columns in table "runcatalog" in database "mysql"
   printf("\nList all columns in table \"runcatalog\" in database \"test\" on server %s\n",
          db->GetHost());
   res = db->GetColumns("test", "runcatalog");
   while ((row = res->Next())) {
      printf("%s\n", row->GetField(0));
      delete row;
   }
   delete res;

   // start timer
   TStopwatch timer;
   timer.Start();

   // query database and print results
   const char *sql = "select dataset,rawfilepath from test.runcatalog "
                     "WHERE tag&(1<<2) AND (run=490001 OR run=300122)";
   // const char *sql = "select count(*) from test.runcatalog "
   //                   "WHERE tag&(1<<2)";

   res = db->Query(sql);

   int nrows = res->GetRowCount();
   printf("\nGot %d rows in result\n", nrows);

   int nfields = res->GetFieldCount();
   for (int i = 0; i < nfields; i++)
      printf("%40s", res->GetFieldName(i));
   printf("\n");
   for (int i = 0; i < nfields*40; i++)
      printf("=");
   printf("\n");

   for (int i = 0; i < nrows; i++) {
      row = res->Next();
      for (int j = 0; j < nfields; j++) {
         printf("%40s", row->GetField(j));
      }
      printf("\n");
      delete row;
   }

   delete res;
   delete db;

   // stop timer and print results
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();

   printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
```

```python
from ROOT import TSQLServer, TSQLResult, TSQLRow, TStopwatch


db = TSQLServer.Connect("mysql://localhost/test", "admin", "123456")

print("Server info: %s" % db.ServerInfo())

# list databases available on server
print("")
print("List all databases on server %s" % db.GetHost())
res = db.GetDataBases()
row = res.Next()
while row:
    print("%s" % row.GetField(0))
    row = res.Next()

# list tables in database "test" (the permission tables)
print('')
print('List all tables in database "test" on server %s' % db.GetHost())
res = db.GetTables("test")
row = res.Next()
while row:
    print("%s" % row.GetField(0))
    row = res.Next()

# list columns in table "runcatalog" in database "mysql"
print('')
print('List all columns in table "runcatalog" in database "test" on server %s' %
      db.GetHost())
res = db.GetColumns("test", "runcatalog")
row = res.Next()
while row:
    print("%s" % row.GetField(0))
    row = res.Next()

# start timer
timer = TStopwatch()
timer.Start()

# query database and print results
# sql = "select dataset,rawfilepath from test.runcatalog " \
#       "WHERE tag&(1<<2) AND (run=490001 OR run=300122)"
sql = "select count(*) from test.runcatalog " \
      "WHERE tag&(1<<2)"

res = db.Query(sql)

nrows = res.GetRowCount()
print("")
print("Got %d rows in result" % nrows)

nfields = res.GetFieldCount()
for i in range(nfields):
    print("%40s" % res.GetFieldName(i))
print("")
print("=" * (nfields * 40))
print("")

for i in range(nrows):
    row = res.Next()
    for j in range(nfields):
        print("%40s" % row.GetField(j))
    print("")

# stop timer and print results
timer.Stop()
rtime = timer.RealTime()
ctime = timer.CpuTime()

print("")
print("RealTime=%f seconds, CpuTime=%f seconds" % (rtime, ctime))
```

## filldb

```cpp
void sqlfilldb(int nfiles = 1000)
{
   const char *ins = "INSERT INTO runcatalog VALUES ('%s', %d,"
      " %d, %d, %d, %10.2f, '%s', '%s', '1997-01-15 20:16:28',"
      " '1999-01-15 20:16:28', '%s', '%s')";

   char sql[4096];
   char dataset[32];
   char rawfile[128];
   int  tag, evt = 0;

   // open connection to MySQL server on localhost
   TSQLServer *db = TSQLServer::Connect("mysql://localhost/test", "admin", "123456");
   TSQLResult *res;

   // first clean table of old entries
   res = db->Query("DELETE FROM runcatalog");
   delete res;

   // start timer
   TStopwatch timer;
   timer.Start();

   // fill run catalog
   for (int i = 0; i < nfiles; i++) {
      sprintf(dataset, "testrun_%d", i);
      sprintf(rawfile, "/v1/data/lead/test/run_%d.root", i);
      tag = int(gRandom->Rndm()*10.);
      sprintf(sql, ins, dataset, i, evt, evt+10000, tag, 25.5, "test", "lead",
              rawfile, "test run dummy data");
      evt += 10000;
      res = db->Query(sql);
      delete res;
      //printf("%s\n", sql);
   }

   delete db;

   // stop timer and print results
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();

   printf("\n%d files in run catalog\n", nfiles);
   printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
```


```python
from ROOT import TSQLServer, TSQLResult, TStopwatch, gRandom


nfiles = 1000

ins = "INSERT INTO runcatalog VALUES ('%s', %d," \
      " %d, %d, %d, %10.2f, '%s', '%s', '1997-01-15 20:16:28'," \
      " '1999-01-15 20:16:28', '%s', '%s')"

tag = evt = 0

# open connection to MySQL server on localhost
db = TSQLServer.Connect("mysql://localhost/test", "admin", "123456")

# first clean table of old entries
res = db.Query("DELETE FROM runcatalog")

# start timer
timer = TStopwatch()
timer.Start()

# fill run catalog
for i in range(nfiles):
    dataset = "testrun_%d" % i
    rawfile = "/v1/data/lead/test/run_%d.root" % i
    tag = int(gRandom.Rndm() * 10.)
    sql = ins % (dataset, i, evt, evt + 10000, tag, 25.5, "test", "lead",
                 rawfile, "test run dummy data")
    evt += 10000
    res = db.Query(sql)
    # print("%s" % sql)


# stop timer and print results
timer.Stop()
rtime = timer.RealTime()
ctime = timer.CpuTime()

print("")
print("%d files in run catalog" % nfiles)
print("RealTime=%f seconds, CpuTime=%f seconds" % (rtime, ctime))
```




<!-- rootsql.md ends here -->
