Petshion
===
팀: Was it a cat I saw?
---
# prototype
>## install
>>1. Visual Studio 2017
>>2. CUDA toolkit 10.0
>>3. CuDNN 7.6.1 (for CUDA 10.0)
>>4. OpenCV 4.1.1
>>5. DLIB 19.17
>## start
>> MFC - Dialog based project
>> Import Library & include folder
>> Select mode(camera, image, video, desktop window)
>> Set frame refresh delay
>> Click "ON"
>> It may work with all extension(test file: *.jpg, *.bmp, *.png, *.avi, *.wav, *.mp4)

# python
> 
>## install
>>1.keras
>><pre>pip install keras</pre>
>>2.cv2
>><pre>pip install opencv-python</pre>
>>3.dlib<br>
>>http://dlib.net/python/index.html
>## start
>>1. <pre>python datasets.py</pre>
>>2. revise that
> 1. 파일 형식을 jpg로
>>3. <pre>python train.py</pre>
>> 이미 모델을 넣어놔서 안해도 됨 하지만 정확도 면에서 부족하기 때문에 정확한 데이터를 가지고
>>학습시켜야 한다. 
>>4. <pre>python test.py</pre>
>

# camera
>## android file
>load model in adnroid
