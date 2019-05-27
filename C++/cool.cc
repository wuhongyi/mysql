// cool.cc --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 一 5月 27 10:21:28 2019 (+0800)
// Last-Updated: 一 5月 27 14:15:22 2019 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 24
// URL: http://wuhongyi.cn 

// g++ cool.cc `mysql_config --cflags --libs`  `root-config --cflags --glibs` -l Spectrum -o cool

#include <mysql/mysql.h>
#include <stdio.h>
#include <iostream>
#include "TGraph.h"
#include "TString.h"
#include <ctime>
#include <string>
// #include "TMultiGraph.h"
#include "RVersion.h"//版本判断
#include "TApplication.h"
// #include "TDatime.h"
#include "TAxis.h"


time_t StringToDatetime(std::string str)
{
  char *cha = (char*)str.data();             // 将string转换成char*。
  tm tm_;                                    // 定义tm结构体。
  int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
  sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// 将string存储的日期时间，转换为int临时变量。
  tm_.tm_year = year - 1900;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
  tm_.tm_mon = month - 1;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
  tm_.tm_mday = day;                         // 日。
  tm_.tm_hour = hour;                        // 时。
  tm_.tm_min = minute;                       // 分。
  tm_.tm_sec = second;                       // 秒。
  tm_.tm_isdst = 0;                          // 非夏令时。
  time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
  return t_;                                 // 返回值。 
}

int main(int argc, char *argv[])
{
  // Create an interactive ROOT application
  TApplication *theApp = new TApplication("Rint", NULL, NULL);
  
  MYSQL       *conn;
  MYSQL_RES   *result;
  MYSQL_ROW    row;
  char        *w;

  // TMultiGraph *gg = new TMultiGraph;
  TGraph *g1 = new TGraph();
  TGraph *g2 = new TGraph();
  int counter = 0;
  
  conn = mysql_init(NULL);
  mysql_real_connect(conn, "222.29.111.176", "data", "123456", "monitor", 3306, NULL, 0);

  if(argc == 1)
    mysql_query(conn, "select * from cool order by ts desc limit 1000");
  else
    mysql_query(conn, TString::Format("select * from cool order by ts desc limit %s",argv[1]).Data());
  
  result = mysql_store_result(conn);
  if (result == NULL)
    {
      printf("%d:%s\n", mysql_errno(conn), mysql_error(conn));
      goto out;
    }


  while ((row = mysql_fetch_row(result)))
    {
      w = row[0];
      std::cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<<std::endl;

      time_t xx = StringToDatetime(row[0]);
      TString y1(row[1]);
      TString y2(row[2]);
      int yy1 = y1.Atoi();
      int yy2 = y2.Atoi();
      double yyy1,yyy2;
      if(yy1 > 32768) yy1 = yy1-65536;
      if(yy2 > 32768) yy2 = yy2-65536;
      yyy1 = yy1/10.0;
      yyy2 = yy2/10.0;

      g1->SetPoint(counter,xx,yyy1);
      g2->SetPoint(counter,xx,yyy2);
      counter++;
    }

  // gg->Add(g1);
  // gg->Add(g2);
  // gg->Draw("APL");
  g1->GetYaxis()->SetRangeUser(-50,30);
  g1->GetXaxis()->SetTimeDisplay(1);
  g1->SetMarkerColor(2);
  g2->SetMarkerColor(3);
  g1->Draw("AP");
  g2->Draw("P same");
  // std::cout<<"============================================"<<std::endl;
  
 out:
  mysql_free_result(result);
  mysql_close(conn);
  mysql_library_end();
  // This function finalizes the MySQL library.
  // Call it when you are done using the library (for example, after disconnecting from the server).
  // The action taken by the call depends on whether your application is linked to the MySQL client library or the MySQL embedded server library.
  // For a client program linked against the libmysqlclient library by using the -lmysqlclient flag, mysql_library_end() performs some memory management to clean up.

  // and enter the event loop...
  theApp->Run();
  delete theApp;
  return 0;
}

// 
// cool.cc ends here
