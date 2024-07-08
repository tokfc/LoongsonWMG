2024创新芯未来嵌入式应用大赛作品

主要运行程序:
python3 LoongsonWMG_BLEclient.py


等待初始化后即可使用触摸屏

data_from_camara
这个文件夹存放人脸训练数据
存放以
ID.Name.Phonenum
格式的文件夹，里面存放该人的人脸照片
“.”分割三个字符串来识别
ID要递增
Name不是带点都可以
Phonenum建议用四位尾号

建立好这个照片库后到主目录运行 
python3 train.py
生成 Facelist.txt 和数据模型 face_trainer.yml
可以在速度更快的PC端运行后传输到龙芯派中
