
write写完文件，如果在不重新打开文件的情况下回读内容请不要忘记用seek移动文件指针

write写完文件，如果在close和sync之前断电最近写入的内容可能无法保存到flash

