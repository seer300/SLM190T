####目录说明
Allbins：生成的bin文件及map文件
ap_boot：AP核的boot，主要负责AP大版本的搬移和启动，以及FOTA升级子分支
cp_boot：CP核的boot，主要负责CP大版本的搬移和启动
TOOLS：NV工具以及死机分析trace脚本
xinyiNBIot_AP：AP大版本源码，裸核形式，根据不同的project工程选择实现不同的版本功能
xinyiNBIot_AP_OS：带OS的AP大版本源码，目前尚未达到商用
.gitignore：git忽略配置
define.cmake：编译宏定义配置，前置文件为.config
git免密码.bat：git免密码脚本，以每次更新git时无需再输入密码
Kconfig：menuconfig的编辑菜单项，增加menuconfig配置项时才需打开编辑
xybuild.exe：编译工具
xybuild.py：编译工具xybuild.exe的源码
xyconfig.exe：做库和sections配置工具，以及menuconfig用户配置