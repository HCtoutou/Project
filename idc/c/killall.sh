##################################
# 停止数据中心后台服务程序的脚本 #
##################################

killall -9 procctl

killall gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles tcpputfiles tcpgetfiles fileserver

killall obtcodetodb obtmindtodb execsql dminingmysql

sleep 3

killall -9 gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles tcpputfiles tcpgetfiles fileserver

killall -9 obtcodetodb obtmindtodb execsql dminingmysql
