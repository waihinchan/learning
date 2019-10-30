Pixel outline test

``` javascript
<!DOCTYPE HTML>
<html>
  <head>
    <style>
      body {
        margin: 0px;
        padding: 0px;
      }
    </style>
  </head>
  <body>
    <canvas id="myCanvas" width="400" height="400"></canvas>
    <canvas id="myCanvas2" width="400" height="400"></canvas>
    <h1>Image Gradients</h1>
    
    <p style="margin:10px;">This code shows you how to get the gradient of an image in both X and Y.</p>
    <p style="margin:10px;"> We're using a 'kernel' of -1 0 1 in both X and Y</p>
    <p style="margin:10px;"> But we're taking a short-cut (no convolution kernel - just manual multiplication of pixels)</p>
    <p style="margin:20px;">imageData2.data[((imageWidth * i) + j) * 4] = (-1*(data[((imageWidth * i) + j-1) * 4])) + (data[((imageWidth * i) + j+1) * 4]);
</p>
    <p style="margin:10px;"> Much simpler to code and less messing about</p>
    
        <p style=“margin:10px;”>Can you calculate the magnitude of the gradients?</p>

    
    <script>
        var imageObj = new Image();
        imageObj.src = "road.jpg";
//常规操作不用解释
        var canvas = document.getElementById('myCanvas');
        var canvas2 = document.getElementById('myCanvas2');
//两个画布
//[HTML5 Canvas | 菜鸟教程](https://www.runoob.com/html/html5-canvas.html)
        var context = canvas.getContext('2d');
        var context2 = canvas2.getContext('2d');
//创建两个画布对象
        var imageWidth = imageObj.width;
        var imageHeight = imageObj.height;
//给一个变量是宽和高

        context2.drawImage(imageObj, 0, 0);
//[HTML canvas drawImage() 方法 | 菜鸟教程](https://www.runoob.com/tags/canvas-drawimage.html)
//这个就是第二个画布的图像，源图
        var imageData = context2.getImageData(0, 0, imageWidth, imageHeight);
//[HTML canvas getImageData() 方法 | 菜鸟教程](https://www.runoob.com/tags/canvas-getimagedata.html)
//这个有点像是，把从坐标值0，0，开始，长宽各自为照片宽度和高度的像素值储存一下
        var data = imageData.data;
//data数列等于这个图片中像素的rgba值
        
        var imageData2 = context.getImageData(0, 0, imageWidth, imageHeight);
//这个imagedata2这时候应该是空的数值，因为它所在的位置现在是没有任何东西的，注意这个时候这些东西都只是一个像素点，而不是rgba数值
//前期准备工作就好了，下面精华来了
```
- - - -
## 先搞懂一个东西

### 复习一个概念：
数组是从[0]开始的[JavaScript Array 对象 | 菜鸟教程](https://www.runoob.com/jsref/jsref-obj-array.html)
也就是[0],[1],[2],[3]等等等等
也就是说[0]才是第一个数，而[1]是第二个数。

### 像素的组成
如果像素看作一个棋盘，比如说一个总像素值为100的正方形区域的话，那它就是由10*10方格组成的。
如何确定某一个像素值的位置呢，参照下图⬇️⬇️⬇️
![](https://tva1.sinaimg.cn/large/006y8mN6gy1g8gq3p3wv1j31400u0kch.jpg)
比如说，第7行第8列像素点。第一直觉是7*8，但是实际上第56个像素点并不是第7行第8列的像素，而是处于第5行第6列。
这也就是上面图片和公式的由来。
``` javascript
Imagedata = i*imagewidth + j
```
### 矩阵的计算方法
![](https://tva1.sinaimg.cn/large/006y8mN6gy1g8gq3pqyzkj30g80is0sz.jpg)
![](https://tva1.sinaimg.cn/large/006y8mN6gy1g8gq3q4m7zj30b40b4q7o.jpg)
![](https://tva1.sinaimg.cn/large/006y8mN6gy1g8gq3qi2prj30b40b443q.jpg)

案例中用的是一个叫outline的内核。也就是内核中央是8，周围各自为-1。
这个要如何计算呢，我们把整个图片看作一个3*3的像素区域。那依次他们每一个像素在数组中的序号就分别为[0],[1],[2],[3],[4],[5],[6],[7],[8]。
那处于最中心的像素点序号则为[5]，也就是8的那个位置。
根据这个矩阵的公式，我们就可以得到，[5] =-1* ([0]+[1]+[2]+[3]+[4]+[6]+[7]+[8]) + 8*[5]。
有一点需要注意的是，我们其实是有两个图片的，第二张是源图，上面那张是效果图。
同理在上面的等式中，左边的[5]是指你即将要生成的[5]，而右边的所有序号的来源都来自于没有经过效果变化前的也就是第二张图片源图像的像素值。
这样这一个像素点就确认下来了。以此类推，我们使用for循环来遍历所有的像素点，让生成的图片中所有的像素都有都根据上面的等式来生成。
- - - -
如果对于↘️↗️↖️↙️⬆️⬇️⬅️➡️这几个像素点的值不知道怎么取的话，可以参照下图
![](https://tva1.sinaimg.cn/large/006y8mN6gy1g8gq42pwm0j31400u04gi.jpg)
![](https://tva1.sinaimg.cn/large/006y8mN6gy1g8gq3p3wv1j31400u0kch.jpg)
- - - -
### 精华部分代码
``` javascript
        for(var I = 1; i < imageHeight-1; i++) {         
            
/////_____________________________________________________//////
			   for(var j = 1; j < imageWidth-1; j++) {
				var left = j - 1;
				var right = j + 1;
             var top = (i-1)*imageWidth;           
				var bottom = (I+1)*imageWidth; 
/////_____________________________________________________//////
 				imageData2.data[((i*imageWidth)+j)*4+0] = (8*data[((i*imageWidth)+j)*4])-data[((i-1)*imageWidth+j)*4]-data[((i+1)*imageWidth+j)*4]-data[((i*imageWidth)+j-1)*4]-data[((i*imageWidth)+j+1)*4]-data[(((i-1)*imageWidth)+j-1)*4]-data[(((i-1)*imageWidth)+j+1)*4]-data[(((i+1)*imageWidth)+j+1)*4]-data[(((i+1)*imageWidth)+j-1)*4];//r
                 
                imageData2.data[((i*imageWidth)+j)*4+1] = (8*data[((i*imageWidth)+j)*4+1])-data[((i-1)*imageWidth+j)*4+1]-data[((i+1)*imageWidth+j)*4+1]-data[((i*imageWidth)+j-1)*4+1]-data[((i*imageWidth)+j+1)*4+1]-data[(((i-1)*imageWidth)+j-1)*4+1]-data[(((i-1)*imageWidth)+j+1)*4+1]-data[(((i+1)*imageWidth)+j+1)*4+1]-data[(((i+1)*imageWidth)+j-1)*4+1];//g
                 
                imageData2.data[((i*imageWidth)+j)*4+2] = (8*data[((i*imageWidth)+j)*4+2])-data[((i-1)*imageWidth+j)*4+2]-data[((i+1)*imageWidth+j)*4+2]-data[((i*imageWidth)+j-1)*4+2]-data[((i*imageWidth)+j+1)*4+2]-data[(((i-1)*imageWidth)+j-1)*4+2]-data[(((i-1)*imageWidth)+j+1)*4+2]-data[(((i+1)*imageWidth)+j+1)*4+2]-data[(((i+1)*imageWidth)+j-1)*4+2];//b
                 
                imageData2.data[((i*imageWidth)+j)*4+3] = 255;//a
//为了显示清楚这里没有用简写，比如说+8*xxx可以用*8+=的写法。同时上面的变量top、right等也可以替换进去（目前我测试如果直接定义变量写进去是有问题的，暂时不知道原因）
            
               
          }
        }
 context.putImageData(imageData2,0,0);




    </script>
  </body>
</html>     
```
