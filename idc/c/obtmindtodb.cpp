/*
 * obtmindtodb.cpp,本程序用于把全国站点分钟观测数据保存到T_ZHOBTMIND表中，支持xml和csv两种文件格式。
 * 作者：Hctoutou
 */

#include "idcapp.h"

CLogFile logfile;  // 日志文件对象

connection conn;  // 数据库连接对象

CPActive PActive;

// 业务处理主函数
bool _obtmindtodb(char *pathname,char *connstr,char *charset);

void EXIT(int sig);

int main(int argc,char *argv[])
{
  // 帮助文档
  if (argc!=5)
  {
    printf("\n");
    printf("Using:./project/tools1/bin/procctl 10 /project/idc1/bin/obtmindtodb /idcdata/surfdata \"127.0.0.1,root,123456,mysql,3306\" utf8 /log/idc/obtmindtodb.log\n\n");

    printf("本程序用于把全国站点分钟观测数据保存到数据库的T_ZHOBTMIND表中，数据只插入，不更新。\n");
    printf("pathname 全国站点分钟观测数据文件存放的目录。\n");
    printf("connstr 数据库连接参数：ip,username,password,dbname,port\n");
    printf("charset 数据库的字符集。\n");
    printf("logfile 本程序运行的日志文件名。\n");
    printf("程序每10秒运行一次，由procctl调度。\n\n\n");

    return -1;
  }

  // 处理程序退出的信号
  // 关闭全部的信号和输入输出
  // 设置信号，在shell状态下可用"kill + 进程号"正常终止这些进程。
  // 但请不要用"kill -9 进程号"强行终止
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // 打开日志文件
  if (logfile.Open(argv[4],"a+")==false)
  {
    printf("打开日志文件失败(%s).\n",argv[4]);
    return -1;
  }
  
  // 进程的心跳，10秒足够
   PActive.AddPInfo(30,"obtmindtodb");
  // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时
  // PActive.AddPInfo(5000,"obtmintodb");

  // 业务处理主函数
  _obtmindtodb(argv[1],argv[2],argv[3]);

  
  return 0;
}


void EXIT(int sig)
{
  logfile.Write("程序退出，sig=%d\n\n",sig);

  conn.disconnect();

  exit(0);
}

// 业务处理主函数
bool _obtmindtodb(char *pathname,char *connstr,char *charset)
{

  CDir Dir;


  // 打开目录
  if (Dir.OpenDir(pathname,"*.xml,*.csv")==false)
  {
    logfile.Write("Dir.OpenDir(%s) failed.\n",pathname);
    return false;
  }

  CFile File;
  CZHOBTMIND ZHOBTMIND(&conn,&logfile);


  int totalcount=0;	// 文件的总记录数
  int insertcount=0;	// 成功插入记录数
  CTimer Timer;		// 计时器，记录每个数据文件的处理耗时。
  bool isxml=false;	// 文件格式，true-xml，false-csv

  while (true)
  {
    // 读取目录，得到一个数据文件名
    if (Dir.ReadDir()==false) break;

    if (MatchStr(Dir.m_FullFileName,"*.xml")==true) isxml=true;
    else isxml=false;

    if (conn.m_state==0)
    {
      if (conn.connecttodb(connstr,charset)!=0)
      {
        logfile.Write("connect database (%s) failed.\n%s\n",connstr,conn.m_cda.message);
        return -1;
      }
      logfile.Write("connect database(%s) ok.\n",connstr);
    }
    
    totalcount=0; insertcount=0;

    // 打开文件。
    if (File.Open(Dir.m_FullFileName,"r")==false)
    {
      logfile.Write("File.Open(%s) failed.\n",Dir.m_FullFileName);
      return false;
    }

    char strBuffer[1001];	// 存放从文件中读取的一行

    while (true)
    {
      if (isxml==true)
      {
        if (File.FFGETS(strBuffer,1000,"<endl/>")==false) break;
      }
      else
      {
        if (File.Fgets(strBuffer,1000,true)==false) break;
        if (strstr(strBuffer,"站点")!=0) continue;	// 把第一行扔掉
      }

      // 处理文件中的每一行。
      totalcount++;

      ZHOBTMIND.SplitBuffer(strBuffer,isxml);

      if (ZHOBTMIND.InsertTable()==true)
        insertcount++;
    }

    // 删除文件，提交事务
    File.CloseAndRemove();
    conn.commit();

    logfile.Write("已处理文件%s(totalcount=%d,insertcount=%d),耗时%.2f秒。\n",Dir.m_FullFileName,totalcount,insertcount,Timer.Elapsed());
  }

  return true;
}


