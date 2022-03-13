/*
 *  Name: crtsurfdata5.cpp 本程序用于生成全国气象站点观测的分钟数据
 *  Author: HCtoutou
 *  将观测数据容器中的记录写入文件  生成xml,json文件 
 */

#include "_public.h"

// 全国气象站点参数结构体
struct st_stcode
{
  char provname[31];  //省
  char obtid[11];     //站号
  char obtname[31];   //站名
  double lat;         //纬度
  double lon;         //经度
  double height;      //海拔高度
};

// 存放全国气象站点参数的容器
vector<struct st_stcode> vstcode;

//把站点参数文件中的参数加载到vstcode容器中
bool LoadSTCode(const char *inifile);

// 全国气象站点分钟观测数据结构
struct st_surfdata
{
  char obtid[11];	// 站点代码
  char ddatetime[21];	// 数据时间：格式yyyymmddhh24miss
  int t;		// 气温：单位，0.1摄氏度
  int p;		// 气压：0.1百帕
  int u;		// 相对湿度，0-100之间的值
  int wd;		// 风向，0-360之间的值
  int wf;		// 风俗，单位0.1m/s
  int r;		// 降雨量：0.1mm
  int vis;		// 能见度：0.1米
};
char strddatetime[21];  // 当前的观测时间
vector<struct st_surfdata> vsurfdata;   // 存放全国气象站点分钟观测数据的容器

// 模拟生成全国气象战点分钟观测数据，存放在vsurfdata容器中
void CrtSurfData();

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件。
bool CrtSurfFile(const char *outpath,const char *dafafmt);

CLogFile logfile;   //日志类

int main(int argc,char *argv[])
{
  // inifile outpath logfile
  if (argc != 5)
  {
    printf("Using:./crtsurfdata5 inifile outpath logfile\n");
    printf("Example:/project/idc1/bin/crtsurfdata5 /project/idc1/ini/stcode.ini /tmp/idc/surdfata /log/idc/crtsurfdata5.log xml,json,csv\n\n");

    printf("inifile 全国气象站点参数文件名。\n");
    printf("outpath 全国气象站点数据文件存放的目录。\n");
    printf("logfile 本程序运行的日志文件名。\n");
    printf("datafmt 生成数据文件的格式，支持xml、json和csv三种格式，中间用逗号分隔。\n\n");

    return -1;
   }
   
   if(logfile.Open(argv[3],"a+",false)==false)
   {
    printf("logfile.Open(%s) failed.\n"); return -1;
   }
   //开始写日志
   logfile.Write("crtsurfdata5 开始运行。\n");
   
   // 把站点参数文件中数据加载到vstcode容器中。
   if(LoadSTCode(argv[1])==false) return -1;

   // 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
   CrtSurfData();

   // 把容器vsurfdata中的全国气象站点分钟观测数据写入文件。
   if (strstr(argv[4],"xml")!=0) CrtSurfFile(argv[2],"xml");
   if (strstr(argv[4],"json")!=0) CrtSurfFile(argv[2],"json");
   if (strstr(argv[4],"csv")!=0) CrtSurfFile(argv[2],"csv");

   logfile.Write("crtsurfdata5 运行结束。\n");

   return 0;
}

// 把站点参数文件中数据加载到vstcode容器中
bool LoadSTCode(const char *inifile)
{
   CFile File;
   // 打开站点参数文件
   if(File.Open(inifile,"r")==false)  // 打开文件失败，写入日志 退出程序
   {
    logfile.Write("File.Open(%s) failed.\n",inifile); return false;
   }

   char strBuffer[301];
   CCmdStr CmdStr;

   struct st_stcode stcode;
   while(true)
   {
    // 从站点参数文件读取一行，如果已经读取完，就跳出循环
    if(File.Fgets(strBuffer,300,true)==false) break; //Fgets会自己初始化字符串
    
    //logfile.Write("=%s=\n",strBuffer);

    //把读取到的一行拆分

    CmdStr.SplitToCmd(strBuffer,",",true); //true表示删除分割后字符前后的空格
    
    if (CmdStr.CmdCount()!=6) continue;   //扔掉无效的行

    // 把站点参数的每个数据项保存到站点参数结构体中
    memset(&stcode,0,sizeof(struct st_stcode));
    CmdStr.GetValue(0, stcode.provname,30);   // 省
    CmdStr.GetValue(1, stcode.obtid,10);      // 站号
    CmdStr.GetValue(2, stcode.obtname,30);    // 站名
    CmdStr.GetValue(3,&stcode.lat);           // 纬度
    CmdStr.GetValue(4,&stcode.lon);           // 经度
    CmdStr.GetValue(5,&stcode.height);        // 海拔高度
    
    // 把站点参数结构体放入站点参数容器
    vstcode.push_back(stcode);
   }
   // 关闭文件
   /*for(int i = 0; i < vstcode.size(); i++)
   logfile.Write("provname=%s,obtid=%s.obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",vstcode[i].provname,vstcode[i].obtid,vstcode[i].obtname,vstcode[i].lat,vstcode[i].lon,vstcode[i].height);*/ 
   return true; 
}

// 模拟生成全国气象站点分钟观测数据 存入vsurfdata容器中
void CrtSurfData()
{
  // 播随机数种子。
  srand(time(0));

  memset(strddatetime,0,sizeof(strddatetime));
  LocalTime(strddatetime,"yyyymmddhh24miss");

  struct st_surfdata stsurfdata;

  // 遍历气象站点参数的vscode容器
  for(int ii = 0; ii < vstcode.size(); ii++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));

    // 用随机数填充分钟观测数据的结构体
    strncpy(stsurfdata.obtid,vstcode[ii].obtid,10);	// 站点代码
    strncpy(stsurfdata.ddatetime,strddatetime,14);	// 数据时间：格式yyyymmddhh24miss
    stsurfdata.t = rand()%351;				// 气温 单位：0.1摄氏度
    stsurfdata.p = rand()%265+1000;			// 气压 0.1百帕
    stsurfdata.u = rand()%100+1;			// 相对湿度 0-100之间的值
    stsurfdata.wd = rand()%360;				// 风向 0-360之间的值
    stsurfdata.wf = rand()%150;				// 风速 单位0.1m/s
    stsurfdata.r = rand()%16;				// 降雨量：0.1mm
    stsurfdata.vis = rand()%5001+100000;		// 能见度：0.1m
    // 把观测数据的结构体放入vsurfdata容器
    vsurfdata.push_back(stsurfdata);
  }
 // printf("aaa\n"); 调试行
}

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件
bool CrtSurfFile(const char *outpath,const char *datafmt)
{
  CFile File;

  // 拼接生成数据的文件名，例如：SURF_ZH_20210629092200_2254.csv
  char strFileName[301];
  sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outpath,strddatetime,getpid(),datafmt);
  // 打开文件
  if (File.OpenForRename(strFileName,"w")==false)
  {
    logfile.Write("File.OpenForRename(%s) failed.\n",strFileName);
    return false;
  }

  // 写入第一行的标题（增加数据文件的可读性）
  if (strcmp(datafmt,"csv")==0) File.Fprintf("站点代码，数据时间，气温，气压，相对湿度，风向，风速，降雨量，能见度\n");
  if (strcmp(datafmt,"xml")==0) File.Fprintf("<data>\n");
  if (strcmp(datafmt,"json")==0) File.Fprintf("{\"data\":[");

  // 遍历存放观测数据的vsurfdata容器
  for(int ii = 0; ii < vsurfdata.size(); ii++)
  {
    // 写入一条记录
    if (strcmp(datafmt,"csv")==0)
    {
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
    }

    if (strcmp(datafmt,"xml")==0)
    {
      File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatatime><t>%.1f</t><p>%.1f</p><u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
    }

    if (strcmp(datafmt,"json")==0)
    {
      File.Fprintf("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\",\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\“,\"r\":、”%.1f、“,\"vis\":\"%.1f\"\"}\"",vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
      if (ii<vsurfdata.size()-1) File.Fprintf(",\n");
      else File.Fprintf("\n");
    }
  }  
  // 写入结束标签
  if (strcmp(datafmt,"xml")==0) File.Fprintf("</data>\n");
  // 关闭文件
  File.CloseAndRename();

  logfile.Write("生成数据文件%s成功，数据时间%s，记录数%d。\n",strFileName,strddatetime,vsurfdata.size());

  return true;

}
