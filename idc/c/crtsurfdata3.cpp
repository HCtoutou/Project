/*
 *  Name: crtsurfdata1.cpp 本程序用于生成全国气象站点观测的分钟数据
 *  Author: HCtoutou
 *  遍历站点参数容器，生成每个站点的观测数据，存放在站点观测数据容器中
 */

#include "_public.h"

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

vector<struct st_surfdata> vsurfdata;   // 存放全国气象站点分钟观测数据的容器

// 模拟生成全国气象战点分钟观测数据，存放在vsurfdata容器中
void CrtSurfData();

CLogFile logfile;   //日志类

int main(int argc,char *argv[])
{
  // inifile outpath logfile
  if (argc != 4)
  {
    printf("Using:./crtsurfdata3 inifile outpath logfile\n");
    printf("Example:/project/idc1/bin/crtsurfdata3 /project/idc1/ini/stcode.ini /tmp/surdfata /log/idc/crtsurfdata3.log\n");

    printf("inifile 全国气象站点参数文件名。\n");
    printf("outpath 全国气象站点数据文件存放的目录。\n");
    printf("logfile 本程序运行的日志文件名。\n");

    return -1;
   }
   
   if(logfile.Open(argv[3],"a+",false)==false)
   {
    printf("logfile.Open(%s) failed.\n"); return -1;
   }
   //开始写日志
   logfile.Write("crtsurfdata3 开始运行。\n");
   
   // 把站点参数文件中数据加载到vstcode容器中。
   if(LoadSTCode(argv[1])==false) return -1;

   // 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
   CrtSurfData();

   logfile.Write("crtsurfdata3 运行结束。\n");

   return 0;
}

// 把站点参数文件中数据加载到vstcode容器中
bool LoadSTCode(const char *inifile)
{
   CFile File;
   // 打开站点参数文件
   if(File.Open(inifile,"r")==false)
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
    
    logfile.Write("=%s=\n",strBuffer);
    //把读取到的一行拆分

    CmdStr.SplitToCmd(strBuffer,",",true);
    
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

void CrtSurfData()
{
  // 播随机数种子。
  srand(time(0));
  // 获取当前时间，当成观测时间。
  char strddatetime[21];
  memset(strddatetime,0,sizeof(strddatetime));
  LocalTime(strddatetime,"yyyymmddhh24miss");

  struct st_surfdata stsurfdata;

  // 遍历气象站点参数的vscode容器
  for(int ii = 0; ii < vstcode.size(); ii++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));

    // 用随机数填充分钟观测数据的结构体
    strncpy(stsurfdata.obtid,vstcode[ii].obtid,10); // 站点代码
    strncpy(stsurfdata.ddatetime,strddatetime,14);  // 数据时间：格式yyyymmddhh24miss
    stsurfdata.t = rand()%351;
    stsurfdata.p = rand()%265+1000;
    stsurfdata.u = rand()%100+1;
    stsurfdata.wd = rand()%360;
    stsurfdata.wf = rand()%150;
    stsurfdata.r = rand()%16;
    stsurfdata.vis = rand()%5001+100000;
    // 把观测数据的结构体放入vsurfdata容器
    vsurfdata.push_back(stsurfdata);
  }
  //printf("aaa\n");
}

