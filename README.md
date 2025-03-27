# SLM190T
美格NB-iot模块 SLM190T   
使用芯翼XY3100E平台开发

## 首次编译前准备
1、解压编译工具包buildtools.zip（SDK随附，芯翼提供）  
解压到`3100E_SDK`目录下，注意目录不要嵌套

2、进入`exclusive-tools`目录，解压好相关开发软件

3、XY3100E编译需要使用到ninja与Python，请提前配置好电脑的环境  
```
#检验环境--打开cmd窗口，输入以下命令
ninja --version
python --version
```

## 开始编译
### Windows平台
进入`3100E_SDK`目录  
运行`xybuild.bat all`开始完整编译  
运行`xybuild.bat clean`清理编译环境  

编译成功后，`3100E_SDK/Allbins`目录下即为编译产物

## 打包固件
在`3100E_SDK`目录下，运行`cp_AllbinsF.bat`脚本  

该脚本会将`Allbins`下的`arm.img cp.img loginfo.info`三个文件拷贝到`V3100EB10001R00C0002\allbins`目录下  

启动LogView软件，执行打包流程。路径选择`V3100EB10001R00C0002\allbins`目录即可

## 分支说明
### master
默认分支
### softsim
移植了来自XY1200S的SoftSIM相关代码  
供Triciti客户使用
