# ReadMe

**描述:**

这个是Kinect v2 在OF 上面运行的一个实例. 可以把边缘检测出后进行点云的效果化.本来是给CSM一个朋友做的舞台效果(开发中).

使用了kinectv2库

[ofTheo/ofxKinectV2](https://github.com/ofTheo/ofxKinectV2)

这个库应该是基于:

[OpenKinect/libfreenect2](https://github.com/OpenKinect/libfreenect2)

所以理论上直接安装下面这个也可以使用 而且多了很多API 但是我没有测试过.

另外还有这么一个库好像也不错但是也未经测试:

[wearenocomputer/ofxNCKinect](https://github.com/wearenocomputer/ofxNCKinect)

实际上在windows上面跑OF还是有很多不错的KinectV2库可以用的,包括微软自带的一些SDK,边缘检测手势检测等.

这里使用了ofxOpenCv,但同时也保留了ofxcv, 前者在轮廓查找器上可以直接用参数确定blob的数量,后者自定义程度更高,还可以根据颜色追踪轮廓.

**部分技术要点:**

使用了帧差技术. 具体可以看这里: 

[framediff](https://openframeworks.cc/ofBook/chapters/image_processing_computer_vision.html)

基于帧差图片后应用opencv的轮廓查找器可以获得比较纯净的效果.

蒙版:

使用了FBO 这个技术我还不是很清楚, 大概原理知道,和我们ps中的蒙版是一个原理. 本来使用的是polyinside或者opencv中的pointpolygontest功能.但是这两个算法非常的缓慢(针对大型不规则多边形), 而使用glsl加速使得性能能够保持在一个比较让人满意的帧率下运作

关于查找轮廓内的点,这个帖子有比较详细的讨论:

[pointinside](https://forum.openframeworks.cc/t/quickest-point-in-ofpolyline-method/13544/11)

**已知问题:**

KinectV2相比于前一代提升了分辨率 但是RGB图像和具有深度信息的RGB图像的尺寸和分辨率是不一致的,具体可以看这里[https://smeenk.com/kinect-field-of-view-comparison/](https://smeenk.com/kinect-field-of-view-comparison/)

1. 由于上述的原因,所以边缘追踪使用的帧差是基于深度图像而非原生的RGB图像的, 且目前这问题没有办法通过resize的方式去解决,因为两个图像之间除了分辨率之间的差别以外,采样的位置也不是从左上角开始的.虽然大胆猜测是居中,但是因为边缘检测和分辨率的影响不是十分大,反而速度相对提升不少. 但由于KinectV2在一些复杂场景或者极端场景下,会损失部分深度信息,导致图片像素丢失(实际上没有丢失,只是没有保留在深度图像的信息中), 在此情况下使用的边缘检测会产生一些噪点.这个问题可以通过侵蚀和扩张得以缓解,但猜测在舞台等纯净环境下该情况会大大改善:

![ReadMe%20c56fd713431c4182b744fad8f99ca9e9/Untitled.png](ReadMe%20c56fd713431c4182b744fad8f99ca9e9/Untitled.png)

此外可以通过设置ROI区域+距离或者IR信息获得更干净的画面.

**进度中:**

1. 对齐点云的mesh位置在地板上(可能需要一些矩阵的矫正) 
2. 添加材质效果(TBD)
