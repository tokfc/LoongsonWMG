import os  
import cv2  
from PIL import Image  
import numpy as np  
  
  
def getImageAndLabel(path):  
    # 人脸数据路径  
    faceSamples = []  
    # id  
    ids = []
    facesids= []
    names = []
    Phones=[]
    #记录脸的个数
    numofFaces=0
    # 获取当前路径的文件夹  
    dirs = os.listdir(path)  
    # 加载分类器  
    faceCascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')  
    # 遍历文件夹  
    for dir in dirs:  
        # 获取文件夹路径  
        dir_path = os.path.join(path, dir)  
        # 获取文件夹下的图片  
        imagePaths = [os.path.join(dir_path, f) for f in os.listdir(dir_path)]  
        # 获取id
        id = int(dir.split('.')[0])
        facesids.append(id)
        names.append(dir.split('.')[1])
        Phones.append(dir.split('.')[2])
        numofFaces = numofFaces+1
        # 遍历图片  
        for imagePath in imagePaths:  
            # 转换为灰度图  
            PIL_img = Image.open(imagePath).convert('L')  
            # 转换为数组  
            img_numpy = np.array(PIL_img, 'uint8')  
            # 人脸检测  
            faces = faceCascade.detectMultiScale(img_numpy)  
  
            if (len(faces) == 0):
                print("没找到人脸在：%s",imagePath)
            else:
                print("找到人脸在：%s",imagePath)
                # 遍历人脸  
                for (x, y, w, h) in faces:  
                    # 添加人脸数据  
                    faceSamples.append(img_numpy[y:y + h, x:x + w])  
                    # 添加id 
                    ids.append(id)
    # 返回人脸数据和id  
    return numofFaces , faceSamples, ids ,facesids, names ,Phones 
  
  
if __name__ == '__main__':  
    # 获取人脸数据和姓名  
    numofFaces, faces, ids, facesids, names ,Phones = getImageAndLabel('data_faces_from_camera')  
  
    # 导入人脸识别模型  
    recognizer = cv2.face.LBPHFaceRecognizer_create()  
  
    # 训练模型  
    recognizer.train(faces, np.array(ids))  
    # 保存模型  
    recognizer.save('./face_trainer.yml')
    print("训练完成！模型保存为./face_trainer.yml")

    # 打开文件
    fo = open("Facelist.txt", "w")
    print ("文件名为: ", fo.name)

    fo.write(str(numofFaces)+"\n")
    for i in range(numofFaces):
        strtoWrite = str(facesids[i])+"."+names[i]+"."+Phones[i] +"\n"
        fo.write( strtoWrite )

    # 关闭文件
    fo.close()
    
    
