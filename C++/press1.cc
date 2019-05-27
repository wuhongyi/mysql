// press1.cc --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 一 5月 27 14:17:32 2019 (+0800)
// Last-Updated: 一 5月 27 14:34:48 2019 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 9
// URL: http://wuhongyi.cn 

// g++ press1.cc `mysql_config --cflags --libs`  `root-config --cflags --glibs` -l Spectrum -o press1

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
#include "TMath.h"

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

  TGraph *g1 = new TGraph();
  int counter = 0;
  
  conn = mysql_init(NULL);
  mysql_real_connect(conn, "222.29.111.176", "data", "123456", "monitor", 3306, NULL, 0);

  if(argc == 1)
    mysql_query(conn, "select * from press1 order by ts desc limit 1000");
  else
    mysql_query(conn, TString::Format("select * from press1 order by ts desc limit %s",argv[1]).Data());
  
  result = mysql_store_result(conn);
  if (result == NULL)
    {
      printf("%d:%s\n", mysql_errno(conn), mysql_error(conn));
      goto out;
    }


  while ((row = mysql_fetch_row(result)))
    {
      w = row[0];
      std::cout<<row[0]<<"  "<<row[1]<<std::endl;

      time_t xx = StringToDatetime(row[0]);
      TString y1(row[1]);
      unsigned int yy1 = y1.Atoi();

      short data1,data2,data3,data4;
      data1 = ((yy1&0xFF000000)>>24)-48;
      data2 = ((yy1&0x00FF0000)>>16)-48;
      data4 = (yy1&0x000000FF)-48;
      data3 = ((yy1&0x0000FF00)>>8);//43+ 45-

      double yyy1;
      if(data3==43)
	{
	  std::cout<<data1<<"."<<data2<<"E+"<<data4<<std::endl;
	  yyy1 = (data1*1.0+data2*0.1)*TMath::Power(10,data4);
	}
      else
	{
	  std::cout<<data1<<"."<<data2<<"E-"<<data4<<std::endl;
	  yyy1 = (data1*1.0+data2*0.1)*TMath::Power(10,-data4);
	}
    
      
      g1->SetPoint(counter,xx,yyy1);
      counter++;
    }


  // g1->GetYaxis()->SetRangeUser(-50,30);
  g1->GetXaxis()->SetTimeDisplay(1);
  g1->SetMarkerColor(2);
  // g1->SetMarkerStyle(1);
  g1->Draw("AP");

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
// press1.cc ends here
