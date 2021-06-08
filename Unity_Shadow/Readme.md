# shadows

啃了好几天关于Unity关于阴影的实现方式和教程，这里从流程和实现细节做一些记录。中间还是有一些似是而非的东西没有搞懂。本文持续更新。。

目前更新到方向光的部分。

## 流程

因为阴影方式其实在SRP下可以自己配置。然后结合到RP来看就会非常混乱。所以这里把着色器流程和提交CommandBuffer的流程分开来写。

### 着色器流程

### 接收阴影

阴影在理解上应该是光照射不到的地方。因为类似于BRDF这种光照模型都需要获得光的方向来计算物体表面的受光情况。所以如果不考虑被光遮挡的因素的话，就很难判断物体的层级关系。

而实现某一个光是否在阴影下的方式是通过计算光的衰减程度来实现的。所以对应的关系应该是

**Light Direction - CBUFFER**

**Light Attenuation - 逐片元计算**

所以就可以在同一束光，在不同物体上得到不同的最终光照效果的实现。即哪怕物体的某一个表面的NDOTL是大于0的，但是加入了光的衰减系数之后仍然会出现结果为0的情况。这个情况就是物体处于阴影之下（受到其他物体遮蔽）。

如果用兰伯特模型的话应该是这样：

```glsl
saturate(dot(surface.normal, light.direction) * light.attenuation) * light.color;
```

### 输出阴影

接下来讲如何获取物体是否被遮蔽。即如何获得light.attenuation

**Unity的做法是通过比较深度值来判断物体是否被遮蔽的**。

通常来说在我们眼睛（相机）所看到的东西的内容会有一个深度值。同样把一个假“相机”移动到灯光的位置，也可以获取一个深度值

因为方向光是没有确切位置的。所以Unity会通过一个函数**ComputeDirectionalShadowMatricesAndCullingPrimitives**（这里的细节会在脚本的部分再讲一下）来把假“相机”设置为正交相机，同时缩放到一个可以涵盖到所有物体的距离内，返回基于灯光的假相机的viewspace矩阵和投影准备矩阵。关于观察矩阵和投影准备矩阵可以去看冯乐乐的书。这里不过多赘述。

由于在shader中会通过ViewMatrix把世界坐标和相机的Z坐标轴对齐，所以可以直接获取z作为深度。（注意这个和到相机的距离不一样。。这里就只是那view space下的z值，即有可能出现在同一个水平截面上的深度一样但是距离相机不同的情况。）同理对于假相机而言，物体也有一个对应的深度值。

下图是方向光渲染出来的深度贴图（经过观察矩阵和投影矩阵后的贴图）。

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled.png)

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%201.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%201.png)

**为了避免引起歧义，下文做如下声明：**

1. **眼睛 = 真正的相机，即Unity里面的相机**
2. **灯光、假相机 = 经过观察者矩阵转换后的相机，只是以某一个特殊的“视觉”去看场景中的物体**
3. **相机 = 代指所有经过观察者/投影矩阵转换后的空间，包括真正的相机和“假相机”。**

实时上获取了深度值我们还需要进一步的计算才可以得到物体是否被遮蔽。也就是使用物体的裁剪空间。由于我们每个片段都会有一个裁剪空间下的xy坐标，也就是屏幕空间坐标。我们可以使用这个东西来对阴影贴图采样。如果采样到的结果（即阴影空间下的深度值）和这个裁剪空间下的z坐标也就是在相机空间下的深度值更小，即可视为被遮挡。

*这里可以用占位的思维来想象一下，即原本某个物体面前没有东西被遮挡，那么此时它在灯光的观察者空间下的深度值应该在渲染的时候被写入到阴影贴图里面。如果这时候物体面前出现了一个新的东西，那么由于灯光下遮挡物体比原来的物体离的灯光更“近”，那么此时最终写入这个阴影贴图的深度值是会比原来的深度值更小的（即遮挡物体把原来的物体的“位置”给占据了）。所以此时我们再用裁剪空间下的xy坐标去采样深度贴图得到的坐标的时候，我们就会得到两个深度不一致的情况。从而可以对比此时这个物体到底有没有其他物体遮挡在他面前。注意这个遮挡不是相对于眼睛而言而是相对于灯光而言的。因为我们的目的是找到灯光是否被遮挡，而不是找到眼睛下能不能看到这个东西。*

这里还有一个引起歧义的地方是：

[Rendering 7](https://catlikecoding.com/unity/tutorials/rendering/part-7/)

有一句话是这么描述的：

> We have the depth information of the scene, from the point of view of the camera. We also have this information from the point of view of each light. Of course this data is stored in different clip spaces, but we know the relative positions and orientations of these spaces. So we can convert from one space to the other. This allows us to compare the depth measurements from both points of view. Conceptually, we have two vectors that should end up at the same point. If they do, both the camera and light can see that point, and so it is lit. If the light's vector ends before reaching the point, then the light is blocked, which means that the point is shadowed.

谷歌翻译说的是：（这里的翻译不符合我们上面说的约定，只是翻译出来给大家看看）

> 从相机的角度来看，我们有场景的深度信息。我们也从每盏灯的角度获得了这些信息。当然，这些数据存储在不同的剪辑空间中，但我们知道这些空间的相对位置和方向。所以我们可以从一个空间转换到另一个空间。这使我们能够从两个角度比较深度测量。从概念上讲，我们有两个向量应该在同一点结束。如果他们这样做了，相机和灯光都可以看到那个点，所以它会被点亮。如果光的矢量在到达该点之前结束，则该光被阻挡，这意味着该点被阴影化。

他这个说法是没错的。但是很容易引起误解，即在一开始需要保存眼睛下的深度信息再去和灯光下的深度信息作比较。实际上我们可以省略这一步。即直接把物体的世界坐标转换到灯光下即可。其中一个原因是投影矩阵本身是不可逆的，如果我们用眼睛空间下物体的裁剪坐标是没有办法求出物体在灯光空间下物体的裁剪坐标的。我也是写道后面发现代码实现和我想的不一样才回过头补上前面的声明和这里的描述。

总结一下上面说的流程就是：

1. 保存物体的世界坐标。
2. 通过脚本计算一个返回一个基于灯光的投影矩阵，计算在该投影矩阵下场景的深度信息，渲染出一张阴影贴图
3. 在片段着色器中使用物体的世界坐标作为输入，转换为基于灯光的投影矩阵下，使用其灯光的“屏幕”空间坐标去对阴影贴图进行采样，并把步骤2渲染的深度信息和经过矩阵变换后的深度信息进行对比，并经过一些插值返回一个数值判断这个片段是否处于阴影之下。这一步使用的采样器是阴影采样器。
4. 把步骤三的信息传送到灯光作为衰减系数，计算最终的表面光照信息。

伪代码如下：

```glsl
float4 position_Shadow = mul(_DirectionalShadowMatrice, position_World); 
float ShadowInformation = SAMPLE_TEXTURE2D_SHADOW(_ShadowMap, SHADOWMAP_SAMPLER, float3(position_Shadow.xy, position_Shadow.z)); //这个xy是采样的uv，z是用于对比的数值
light.attenuation = ShadowInformation;
return saturate(dot(surface.normal, light.direction) * light.attenuation) * light.color;
```

这里有一个细节是关于SAMPLE_TEXTURE2D_SHADOW返回的结果到底是什么。查了一下unity URP 的github里面的定义是这样的：

```glsl
#define SAMPLE_TEXTURE2D_SHADOW(textureName, samplerName, coord3) textureName.SampleCmpLevelZero(samplerName, (coord3).xy, (coord3).z)
```

[SampleCmp (DirectX HLSL Texture Object) - Win32 apps](https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-samplecmp)

```glsl
DXGI_FORMAT SampleCmpLevelZero(
  in  SamplerState S,
  in  float        Location,
  in  float        CompareValue,
  in  int          Offset,
  out uint         Status
);
```

这里等于把我们的相机空间下的屏幕空间坐标的xy作为采样坐标，把其z作为对比值。即可用xy采样完后和z对比。如果z大于采样的结果，则为1，否则为0. （同时会根据采样器状态来决定，比如说point，那就是严格按照01来输出的，如果是linear的话，就会对周边的像素进行双线性插值，再*一开始得到的比较结果0和1。这里附一个对比图）:

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%202.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%202.png)

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%203.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%203.png)

图一为SHADOW_SAMPLER sampler_point_clamp_compare

图二为SHADOW_SAMPLER sampler_linear_clamp_compare

point模式，可以看到很硬的边缘，因为只有0和1。而图2因为在边缘处有一些部分是不在阴影下的。此时不在阴影和在阴影下的深度值会进行混合。从而得到了比较柔和的结果

同时贴一个来自于OPENGL对于阴影如何实现的教程的描述：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%204.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%204.png)

至此我们就可以通过在片段着色器中采样阴影数据来把灯光的衰减系数参与到光照渲染的流程之中。

---

### 阴影优化：

其实获取阴影的过程和思路应该不是很难（除了里面涉及到的一些计算细节有点多以外），主要还是在阴影的优化上涉及到比较多的细节要注意。这里主要参考了这里的描述。其实看这篇教程是能看得懂的，但是因为作者喜欢引导大家思考，经常给出一些错误答案，然后再去改正。然后每次修改都要改动三四个脚本和着色器，来回反复横跳。这里看的实在是头大。所以重新整理一下他教程里面的内容

[Directional Shadows](https://catlikecoding.com/unity/tutorials/custom-srp/directional-shadows/)

**阴影级联：**

**原理讲解部分：**

如果直接使用上述的方法直接采样我们得到的结果可能会是这样的：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%205.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%205.png)

转自：[http://www.opengl-tutorial.org/cn/intermediate-tutorials/tutorial-16-shadow-mapping/#rendering-the-shadow-map](http://www.opengl-tutorial.org/cn/intermediate-tutorials/tutorial-16-shadow-mapping/#rendering-the-shadow-map)

这个现象叫做阴影粉刺（也有叫什么痤疮的，感觉有点恶心。。虽然粉刺也挺恶心的），总的来说就是阴影像是一层一层“镶嵌”在物体的表面上了。原因如下图（其实我并没有看懂这个是什么意思。。反正只要知道怎么解决就完事了）

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%206.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%206.png)

缓解这个现象的第一个方法就是级联阴影贴图，注意这个是只有方向光可以用的。因为方向光是一个只有方向没有位置的灯光，类似于聚光灯、点光这一类型的灯光都不是无限远的。例如聚光灯作为假相机采样到时候因为形状是锥形的，所以很符合我们平时用的透视相机的特性。而级联阴影贴图是通过不同的远近关系但是保持正交来实现的。这一点没有办法应用在聚光灯这一类具有确切位置的灯光上（点光源的阴影贴图是立方贴图）。

阴影级联贴图一般来说长成如下这个样子：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%207.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%207.png)

直观来看级联阴影贴图是一个近到远的关系。也就是在一个恒定尺寸下，深度信息的“分辨率”问题。也就是当离得近的时候，深度信息因为总尺寸有限，可以分配到的像素数量更多。而更远的部分则被设置为1（无限远的情况）。

在Unity的实现中其实是按照最大阴影距离来划分这个区域的。这个部分的细节会在脚本里面描述。假设最大阴影距离为1000，如果划分为4个级联级别，每个级别递进25%，那就是 25% ，50% ， 75% ，100%. 即第一个级联信息只有最大距离的25%的信息。

这样划分有助于我们在渲染近处的物体（距离相机近的）获得更好的阴影质量，而远处的阴影因为看不清楚从而可以获得一个大概的数值。

这里的实现方法是把相机进行调整，仍然是使用**ComputeDirectionalShadowMatricesAndCullingPrimitives。**函数会接受一个距离百分比参数来返回对应的投影和观察矩阵。 也就是我们上面提及的25% 50% 75% 100%（注意这个比率是可以调的，最后的一个比率肯定会覆盖到100%的区域）。

这样就等于我们需要渲染N次深度信息，并把其合并到一张阴影贴图里面。 这个N次代表了我们在渲染管线中设置的最大级联数，一般来说是4。

这里怎么样渲染4次并且把其渲染到一张贴图的细节在脚本的部分再描述。反正在shader中我们已经得到了这么一张具有级联信息的大阴影贴图。

以上这个是针对一个灯光的。如果我们有M个灯光都支持阴影，同时每个灯光都使用N个阴影级联，那么我们这个贴图最终会划分为M*N个区域，每N个区域对应一个灯光下的一组级联信息。 因为我们可以让物体受到不止一个灯光的照射，所以自然在shader中我们会通过循环采样获得一个物体在一个场景下受到的所有灯光的影响。为了方便解释，这里都还是只用一个灯光作为例子。多个灯光只有贴图上的区别，对于PBR来说函数只关心灯光自身能给物体的片段带来的影响，并不关心场景中有多少个阴影。下面讲shader的一些实现细节如如何根据距离来选择采样的级联等级，应该读取哪一些贴图和参数等。

**缓冲区**

因为一个场景中可以共享所有灯光的信息。所以我们可以同样的把阴影贴图、灯光列表、阴影级联信息等等这些内容放到一个缓冲区，一般来说我们需要如下的信息：

```glsl
CBUFFER_START(_CustomShadows)
	int _CascadeCount;
	float4 _CascadeCullingSpheres[MAX_CASCADE_COUNT];
	float4x4 _DirectionalShadowMatrices[MAX_SHADOWED_DIRECTIONAL_LIGHT_COUNT * MAX_CASCADE_COUNT];
	float4 _ShadowAtlasSize;
CBUFFER_END

TEXTURE2D_SHADOW(_DirectionalShadowAtlas);
```

1. 级联数_CascadeCount，这个是当前管线使用的设置。因为采样是一张大的阴影贴图，所以我们需要知道有多少个区域，以便shader能够读取到正确的纹理区域。
2. 阴影贴图的尺寸_ShadowAtlasSize，这个没什么好说的。。因为我们有级联，所以自然要计算这些偏移量的多少
3. 阴影投影矩阵_DirectionalShadowMatrices，也就是在采样过程中我们需要把物体的世界坐标转换到在方向光的“假相机”下的正交投影矩阵内进行采样。注意每一个级联都对应一个矩阵，所以实际上这里储存的是一个列表，而非单独的某一个矩阵
4. 采样球体列表_CascadeCullingSpheres，这个是用于阴影在不同级联下如何选择采样的一个“采样器”，即这个球体决定了物体的阴影应该来自于阴影贴图的哪一个级别的级联信息。
5. 阴影贴图，这个就是通过脚本把渲染好的阴影信息发送到GPU缓冲区的一个阴影贴图。注意这个不是放到CBUFFER的，这个在脚本中属于RenderTarget。具体细节在脚本部分再描述。

以上是我们采样一个级联阴影贴图所需要的一些基本信息。后面还会有一些其他优化所需要的信息会加进来。然后做一个汇总。

首先说一下用于选择级联信息的采样球。上文提及到级联的作用可以令近处的物体获得更好的阴影质量。

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%208.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%208.png)

这个球在shader中体现为一个列表。而单个元素的结构为四分量，其中xyz代表了球的坐标。同时w分量代表了这个球体的半径。

然后采样的时候需要根据点到球体的距离来决定采样使用的级联数。伪代码如下

```glsl
int i;
	for (i = 0; i < _CascadeCount; i++) {
		float4 sphere = _CascadeCullingSpheres[i];
		float distanceSqr = DistanceSquared(surfaceWS.position, sphere.xyz);
		if (distanceSqr < sphere.w) {
			break;
		}
	}
	int cascadeIndex = i;
```

这个i就是我们需要的使用的那个级联数。采样球和级联对应的关系是一个级联对应一个球体。只要某个点在这个球体的范围内，也就是在对应的最适合的级联范围内。下图应该会比较直观

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%209.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%209.png)

结合上面说的最基本的流程，加上级联之后的流程如下： 

1. 获取矩阵数组 + 采样球数组 + 阴影级联贴图
2. 迭代N次，通过访问采样球数组里面的采样球来计算当前的点与采样球的距离，如果距离小于球体的半径，则采用该球体下对应的级联数
3. 根据获取的索引访问转换矩阵列表里面矩阵，同时把世界坐标转换为对应该转换坐标下的位置。

**伪代码如下：**

```glsl
float DistanceSquared(float3 pA, float3 pB) {
	return dot(pA - pB, pA - pB);
}
//选定采样级别
int i;

	for (i = 0; i < _CascadeCount; i++) {
		float4 sphere = _CascadeCullingSpheres[i];
		float distanceSqr = DistanceSquared(surfaceWS.position, sphere.xyz);
		if (distanceSqr < sphere.w) {
			break;
		}
	}
//根据索引获取对应的转换矩阵
int cascadeIndex = i;
float4x4 _DirectionalShadowMatrice = _DirectionalShadowMatrices[i];
//采样阴影数据
float4 position_Shadow = mul(_DirectionalShadowMatrice, position_World); 
float ShadowInformation = SAMPLE_TEXTURE2D_SHADOW(_ShadowMap, SHADOWMAP_SAMPLER, float3(position_Shadow.xy, position_Shadow.z)); //这个xy是采样的uv，z是用于对比的数值
light.attenuation = ShadowInformation;
return saturate(dot(surface.normal, light.direction) * light.attenuation) * light.color;
```

关于这些投影矩阵列表和采样球列表的对应关系在脚本部分再介绍。

---

**阴影衰减**

**原理讲解部分：**

在上述的过程中，假设片段某一个部分超过了最远的距离，我们需要把它的设置为0。即当片段超过了最远距离后阴影强度会变为0（注意这个阴影强度可以由我们自定义，因为这里不是重点所以没有展开来讲具体的实现细节，但是这个阴影强度是属于可配置的）。

```glsl
if (i == _CascadeCount) {z
		shadowStrength  = 0.0;
	}
```

这里有一个细节，就是脚本中计算得到的最后一个采样球是会略微大于我们给定的最大阴影距离。则假设_maxShadowDistance是1000，可能最后一个采样球覆盖到的半径会超出1000这个范围。通过对比这两个图可以得出：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2010.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2010.png)

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2011.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2011.png)

我们会发现图一比图二的阴影覆盖面积更大。这种情况一般发生在如果我们把最大阴影距离设置的非常小的时候，会出现这种情况。*（这里的逻辑是，如果我们的最大阴影距离非常小的时候，我们在眼睛空间下可以看得到的阴影层级是很明显的。而通过加强采样球的范围来显示阴影级联呈现的结果会和实际的结果不匹配。因为采样球的距离是会略微大于最大阴影距离的，所以我们会发现加强采样球结果即图1的时候按道理某些区域应该会有阴影，但是实际的渲染中是没有阴影的。这两个图的显示仅仅是一个实验，其实我们不做这个衰减在一般情况下也不会有问题。因为往往相机的最大可视范围和阴影的最大距离都会离得比较远~~。猜测阴影球超出的范围应该是一个常量，否则如果根据最大阴影距离来变化比率的话应该不会出现这个情况。~~**但是我们仍然要解决这个问题。**）*

但是如果直接把大于最大阴影距离的片段的阴影去掉的话，边缘会非常硬。所以边缘强度可以在即将靠近阴影的最大距离时逐渐消失。这样不会出现物体的阴影在某一个距离突然消失的情况。

```glsl
float FadedShadowStrength (float distance, float scale, float fade) {
	return saturate((1.0 - distance * scale) * fade);
} //这里有个细节是把除法在脚本中实现为乘法，可以加快速度。本质上是一样的。iu是distance * （1/scale）而已
```

这里我们需要一个衰减因子和一个衰减开始的范围。即从最大距离前的某个距离开始，直到最大距离，逐渐衰减阴影的强度。

衰减分为两种衰减，一个是根据当前眼睛空间下的深度值和最大阴影距离进行衰减，另外一个是根据级联采样球和片段的距离进行衰减。

深度使用$(1 - d/m)/f$ 实现。d是当前物体的深度值（眼睛空间下，因为要考虑到我们能看见的内容去优化，而非优化那些我们看不到的地方），m是最大阴影距离（这个是属于可配置项），f则是衰减范围，即某个距离开始。

而级联衰减使用$1 - (d^2/ r^2) / F$。这个d是指采样球到片段的距离的平方，r平方则是球的半径的平方。

所以按照惯例我们同样需要一个参数由脚本给出，其中x分量为1 / _maxShadowDistance, y分量为深度衰减因子 1 / _shadowFade，而z分量为级联衰减因子 1 /  (1 - (1 - _cascadeFade) * (1 - _cascadeFade))

级联衰减因子写成这个鬼样是因为他们使用了同一个函数来计算两个衰减，所以为了保证F和f的衰减因子在控制的层面上一样，所以就做了一些显式转换。。这里不用纠结。。。

至此这里的流程再结合上面的级联的流程为：

1. 根据预设的参数从脚本中将其打包成一个四分量，其中xyz分别是如何控制衰减的因素。
2. 加入一个叫阴影强度的参数，用于控制那些边缘部分的阴影质量。
3. 根据深度距离和最大阴影距离来初始化一个阴影强度，这里主要是解决那些在采样球范围内但是超出了最大阴影距离的片段也拥有一定的阴影强度而定的。
4. 在使用采样球确定采样的级联数的时，如果所在片段是在最后一个级联，则对阴影强度加上级联衰减因素。
5. 如果超出了级联数，则设置阴影强度为0。

下面是shader部分：

**缓冲区**

```glsl
CBUFFER_START(_CustomShadows)
float4 _ShadowDistanceFade;
CBUFFER_END
```

实际上是在最后一个级联才会使用这个衰减，也就是当采样球到最后一级时，会使用上述的函数，把片段到球面的距离作为参数distance传递到函数中。也就是

**伪代码如下：**

```glsl
float shadowStrength;
shadowStrength = FadedShadowStrength(depth, _ShadowDistanceFade.x, _ShadowDistanceFade.y); 
//先根据深度初始化衰减因子。在级联中再叠加级联的衰减因子。 
//其实这里如果不想这么麻烦直接设置为1问题也不大。
//这里主要是解决那些在采样球范围内但是超出了最大阴影距离的片段也拥有一定的阴影强度而定的。
int i;

	for (i = 0; i < _CascadeCount; i++) {
		float4 sphere = _CascadeCullingSpheres[i];
		float distanceSqr = DistanceSquared(surfaceWS.position, sphere.xyz);
		if (distanceSqr < sphere.w) {
			if(i == _CascadeCount - 1){ //最后一个级联
				shadowStrength = FadedShadowStrength (distanceSqr ,1.0 / sphere.w,_ShadowDistanceFade.z); 
			}
			
			break;
		}
	}
if (i == _CascadeCount) {
		shadowStrength  = 0.0;
	}
```

---

### 偏差

**原理讲解：**

这里的偏差是用于解决上面阴影粉刺的常用方法。通常分为深度偏差和法线偏差。

深度偏差是指在渲染深度贴图写入深度的时候加一个偏移值bias，这样就可以让物体稍微远离灯光，从而减少粉刺的情况。

而法线偏差则是指把物体的顶点位置沿法线推离，一般来说这里用的是原始法线。如果我们在光照渲染中沿法线推理顶点的话可以得到类似于物体膨胀的效果。

**深度偏差**

伪代码如下：

这个是来自于OpenGL的教程的，在Unity中应用全局深度偏差是通过脚本来实现的。

```glsl
float bias = 0.005;
float visibility = 1.0;
if ( texture( shadowMap, ShadowCoord.xy ).z  <  ShadowCoord.z-bias){
    visibility = 0.5;
}
```

而深度偏差除了以下面这种应用全局的方式以外，还有一个基于斜率应用的偏差，这使得偏差是可变的而非一个常量。

但是使用这个东西的时候会有一个缺点是对于平面会偏离的很厉害，但是对于曲面来说还是多少存在这种粉刺的现象（直观想象一下就是曲面的“坡度”更大，所以还有一部分的粉刺留在里面，但是平面的“坡度”几乎接近于0，所以一点点深度偏差就会让阴影和物体的连接处分离。）如下图所示（源于opengl-tutoria）

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2012.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2012.png)

基于斜率的偏差代码如下：

```glsl
float bias = 0.005*tan(acos(dot(normal,light))); 
bias = clamp(bias, 0,0.01);
```

这个思路就是计算使用法线来和光线组成的夹角的tan值获取斜率。这样在一些平坦的面上斜率较少，所以应用的偏差就小一点。

效果如下：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2013.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2013.png)

在Unity中的应用方式和上述的略有不同，例如没有像上面的伪代码中设置一个偏差上限的问题，因为tan的范围可以到无穷大，所以在设置斜率偏差的系数时需要把数值设置的非常小。深度偏差是通过脚本中的buffer.SetGlobalDepthBias函数来实现的。这个函数接收两个参数，第一个是全局偏差系数，第二个是斜率偏差系数。设置方式在脚本的部分介绍。

---

**法线偏差**

这里我看catlikecoding的实现方式感觉到非常疑惑。这里引入了一个纹素的概念。纹素可以参考这篇链接的解释：

[屏幕像素与纹理像素（Pixel和Texe，像素和纹素l）_aoxuestudy的专栏-CSDN博客](https://blog.csdn.net/aoxuestudy/article/details/110781553)

catlikecoding中使用了纹素的大小作为偏移量。而偏移量的大小是用下面的代码计算出来的：

```glsl
float texelSize = 2f * cullingSphere.w / tileSize * 1.4142136f;
```

然后再对世界坐标 应用上偏移量 * 法线。 即

```glsl
float4 positionSTS = mul(_DirectionalShadowMatrices[index],float4(positionWorld + normalWorld * texelSize, 1.0)
```

我在这里参照了URP的做法，似乎对于法线偏移并不是使用纹素大小来作为偏移量的。URP的做法等一下再贴出来。

这里写一下猜测是URP和defaultRP的做法应该是在渲染阴影贴图的时候就已经应用了法线偏移了。而在catlikecoding里面是在采样的时候才进行偏移的，也就是阴影贴图的数值是没有经过偏移的，偏移的只是采样的时候。猜测的缘由是我翻了一下URP和以前的一些用法，对于shadowCaster的Vertex是这么写的：

```glsl
float4 GetShadowPositionHClip(Attributes input)
{
    float3 positionWS = TransformObjectToWorld(input.positionOS.xyz);
    float3 normalWS = TransformObjectToWorldNormal(input.normalOS);

    float4 positionCS = TransformWorldToHClip(ApplyShadowBias(positionWS, normalWS, _LightDirection));

#if UNITY_REVERSED_Z
    positionCS.z = min(positionCS.z, positionCS.w * UNITY_NEAR_CLIP_VALUE);
#else
    positionCS.z = max(positionCS.z, positionCS.w * UNITY_NEAR_CLIP_VALUE);
#endif

    return positionCS;
}

Varyings ShadowPassVertex(Attributes input)
{
    Varyings output;
    UNITY_SETUP_INSTANCE_ID(input);

    output.uv = TRANSFORM_TEX(input.texcoord, _BaseMap);
    output.positionCS = GetShadowPositionHClip(input);
    return output;
}
```

这个是URP的。

```glsl
float4 MyShadowVertexProgram (VertexData v) : SV_POSITION {
	float4 position = UnityClipSpaceShadowCasterPos(v.position.xyz, v.normal);
	return UnityApplyLinearShadowBias(position);
}
```

这个是catlikecoding的旧教程的。

这两个部分都是在shadowCaster阶段的顶点着色器。然后我又参考了URP对于LightShadowAttenuation的采样过程，也没有找到关于对shadowCoord有什么偏移的地方，参考如下：

```glsl
half ComputeCascadeIndex(float3 positionWS)
{
    float3 fromCenter0 = positionWS - _CascadeShadowSplitSpheres0.xyz;
    float3 fromCenter1 = positionWS - _CascadeShadowSplitSpheres1.xyz;
    float3 fromCenter2 = positionWS - _CascadeShadowSplitSpheres2.xyz;
    float3 fromCenter3 = positionWS - _CascadeShadowSplitSpheres3.xyz;
    float4 distances2 = float4(dot(fromCenter0, fromCenter0), dot(fromCenter1, fromCenter1), dot(fromCenter2, fromCenter2), dot(fromCenter3, fromCenter3));

    half4 weights = half4(distances2 < _CascadeShadowSplitSphereRadii);
    weights.yzw = saturate(weights.yzw - weights.xyz);

    return half(4.0) - dot(weights, half4(4, 3, 2, 1));
}

float4 TransformWorldToShadowCoord(float3 positionWS)
{
#ifdef _MAIN_LIGHT_SHADOWS_CASCADE
    half cascadeIndex = ComputeCascadeIndex(positionWS);
#else
    half cascadeIndex = half(0.0);
#endif

    float4 shadowCoord = mul(_MainLightWorldToShadow[cascadeIndex], float4(positionWS, 1.0));

    return float4(shadowCoord.xyz, 0);
}
```

暂时没有发现应用到纹素和法线参与偏移的过程。不过这里有一些根据采样球采样的方式和上面的for循环不一样的地方，猜测是一些优化的部分避免了使用for循环。

回到对纹素使用的方法的“猜测”部分。首先对于这个部分

```glsl
float texelSize = 2f * cullingSphere.w / tileSize * 1.4142136f;
```

首先这个titleSize是指每一个独立级别用的贴图的尺寸，比如说总贴图大小是1024 * 1024，总共4个灯 * 4个级联，那么等于把贴图分割成16个图块。每个图块的像素量为1024^2 / 16 = 65536。在开平方就得到每个尺寸是256即为titleSize。这个部分会在脚本里面说明。

同时他的教程中提及由于灯光矩阵使用的是正交相机。所以我们最终得到的屏幕空间下的每个像素都应该具有固定的世界大小。即每个像素点能够精确的代表在世界空间下的N个单位。（这也是为什么要使用级联的原因。）

> Because directional lights affect everything up to the max shadow distance their shadow maps end up covering a large area. As the shadow map uses an orthographic projecting each texel in the shadow map has a fixed world-space size. If this size is too large then individual shadow texels are clearly visible, resulting in jagged shadow edges and small shadows that can disappear. This can be alleviated by increasing the atlas size, but only up to a point.
When using a perspective camera things further away appear smaller. At some visual distance a shadow map texel will map to a single display pixel, which means that the shadow resolution is theoretically optimal. Closer to the camera we'd need a higher shadow resolution, while further away a lower resolution would suffice. This suggests that ideally we'd use a variable shadow map resolution, based on the view distance of the shadow receiver.
Cascaded shadows maps are a solution to this problem. The idea is that shadow casters are rendered more than once, so each light gets multiple tiles in the atlas, known as cascades. The first cascade covers only a small region close to the camera and successive cascades zoom out to cover an increasingly larger area with the same amount of texels. The shader then samples the best cascade available for each fragment.

所以当我们使用采样球的直径去除以纹素的大小的时候，我们就可以得到平均每个纹素单位对应的世界空间的距离。（或者可以这么理解，即假设采样球的直径为1000，尺寸区域为256 * 256. 如果我们需要用256个像素点来表示一个长度为1000的数值，那么平均每个像素点都代表了4个单位的长度）。这样再乘上在世界空间下的法线方向的偏移。约等于我们保证了物体在世界空间下沿着法线方向位移的距离是足够远，以至于能满足其转换到灯光下的投影空间时，能够在阴影贴图下找到一个对应的独立的纹素。（好tm拗口。。）然后*根号二倍的原因猜测是因为除了左右上下移动还有可能向对角线方向移动。所以要*上一个根号二倍，保证最小也要移动对角线长度那么多的单位。

整理一下简单而言就是我们要在采样的过程中通过偏移物体片段的世界空间坐标来避免阴影粉刺的现象。而偏移的方式是通过法线。众所周知法线是经过归一化的，所以其实相对于世界空间坐标而言偏移的量不是特别大（虽然视觉上应该是能看得出来的）。而又因为方向光使用的投影矩阵是正交矩阵，从上文得知这会使得最终的屏幕空间会产生一个像素对应一定世界空间范围内距离的现象。即如果我们不应用这个纹素的倍率放大的话，很可能我们应用的这个偏移就应用了一个寂寞。也就是你偏移完还是落在了你本身没有偏移所对应的这个阴影贴图的某一个像素点上。这个我们可以通过做一个小小的实验来测试一下。即把代码

```glsl
float4 positionSTS = mul(_DirectionalShadowMatrices[index],float4(positionWorld + normalWorld * texelSize, 1.0)；
```

改成

```glsl
float4 positionSTS = mul(_DirectionalShadowMatrices[index],float4(positionWorld + normalWorld, 1.0)
```

也就是把这个纹素的偏移量给去除掉。

结果如下：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2014.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2014.png)

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2015.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2015.png)

图一是灯光normalbias属性为0.01的情况。而图二为灯光normalbias属性为0.5的情况。可以图一看出来这里应用了个寂寞。而图二因为0.5已经极大地超过了正常应该有的偏移量，反而导致了不该有阴影的地方出现了阴影。也就是说上面说的这个texelSize的作用其实是保证我们每一次偏移都是有“意义”的，而对于单独每一个灯光独立的normalbias，可以作为我们控制的一个参数。这使得最终我们的结果具有更强的可控性。

**缓冲区**

上述的原理中我们总共需要如下的一些属性：

1. 深度偏移depthbias
2. 斜率偏移slopbias
3. 法线偏移最小量texelsize
4. 逐灯光法线偏移量light.normalbias

因为1 和 2都可以通过脚本的形式来设置，然后Unity提交command buffer的时候会自动应用这一部分的偏移，这部分不在shader的配置范围内。（我们也可以自己多搞两个变量来在shader里面配置，比如说参照上面openGL教程方式手动去做比较）

而第三个texelsize会根据采样球的半径和我们在阴影的级联设置中使用的级联数有关。所以这个部分需要由脚本把数据传递到一个数组_CascadeData。然后依次访问_CascadeData的索引来获取每个采样球下应该应用的偏移量。同时_CascadeData可以储存我们在上面提及到的阴影衰减的x也就是1.0 / sphere.w。

而第四个储存在每个灯光的数组的属性中（注意是阴影灯才具有这个属性，如果灯光不投射阴影这个也没什么意义）

```glsl
CBUFFER_START(_CustomShadows)
float4 _CascadeData[MAX_CASCADE_COUNT];
CBUFFER_END
```

**伪代码**

```glsl
float normalbias = _CascadeData[index].y * normal_WS * light.normalbias;
float4 position_Shadow = mul(_DirectionalShadowMatrice, position_World + normalbias ); 
//下面的部分和最初采样的部分是一样的。
float ShadowInformation = SAMPLE_TEXTURE2D_SHADOW(_ShadowMap, SHADOWMAP_SAMPLER, float3(position_Shadow.xy, position_Shadow.z)); //这个xy是采样的uv，z是用于对比的数值
light.attenuation = ShadowInformation;
return saturate(dot(surface.normal, light.direction) * light.attenuation) * light.color;
```

至此我们就完成了对法线向量的偏移。

### **软阴影**

实现软阴影的一个方式就是通过PCF多次采样从而获取比较柔和的阴影的效果。注意这里和上面提到的SHADOW_SAMPLER sampler_linear_clamp_compare 和 SHADOW_SAMPLER sampler_point_clamp_compare不是同一个东西。PCF和这里采样阴影的方式的效果是互相独立的，虽然从结果来看有一点点相似。

> PCF
The easiest way to improve this is to change the shadowmap’s sampler type to sampler2DShadow. The consequence is that when you sample the shadowmap once, the hardware will in fact also sample the neighboring texels, do the comparison for all of them, and return a float in [0,1] with a bilinear filtering of the comparison results.
For instance, 0.5 means that 2 samples are in the shadow, and 2 samples are in the light.
Note that it’s not the same than a single sampling of a filtered depth map ! A comparison always returns true or false; PCF gives a interpolation of 4 “true or false”.

关于为什么要用PCF和一些其他平台的教程可以参考这里：

[OpenGL 3D Game Tutorial 40: Percentage Closer Filtering](https://www.youtube.com/watch?v=TcbvOvmLSow)

**2021-06-07更新**

关于PCF算法其实类似于对图片应用一个卷积核来进行采样。而根据提到的：

[Chapter 11. Shadow Map Antialiasing](https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-11-shadow-map-antialiasing)

> Note that the sampling region is not aligned to texel boundaries. An aligned region would not achieve the antialiasing effect that we want.

也就是说如果我们直接和纹素对齐是不能获取软阴影的效果的，所以是在两个像素之间进行插值。如下：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2016.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2016.png)

其实卷积核的概念是很好理解的，参考：

[Image Kernels explained visually](https://setosa.io/ev/image-kernels/)

[Image convolution examples](https://aishack.in/tutorials/image-convolution-examples/)

而Unity里面关于tent filter有点复杂的其中一个原因就是像素点不是和卷积核中心对齐。而且其加权的方式是通过三角形覆盖的纹素面积而定的。

然后我花了很多的心思和画图去解读这个temt filter的一些实现细节。这里只能给出一个大概的思路，具体的代码实现的细节就不一一展开了。其实这里读源码和画图的过程还是挺有意思的，给了一个教训就是源代码的注释一定要一行一行的去看！！

首先是基于原教程下的写法是这样的

[Directional Shadows](https://catlikecoding.com/unity/tutorials/custom-srp/directional-shadows/)

```glsl
float FilterDirectionalShadow (float3 positionSTS) {
	#if defined(DIRECTIONAL_FILTER_SETUP)
		float weights[DIRECTIONAL_FILTER_SAMPLES];
		float2 positions[DIRECTIONAL_FILTER_SAMPLES];
		float4 size = _ShadowAtlasSize.yyxx;
		DIRECTIONAL_FILTER_SETUP(size, positionSTS.xy, weights, positions);
		float shadow = 0;
		for (int i = 0; i < DIRECTIONAL_FILTER_SAMPLES; i++) {
			shadow += weights[i] * SampleDirectionalShadowAtlas(
				float3(positions[i].xy, positionSTS.z)
			);
		}
		return shadow;
	#else
		return SampleDirectionalShadowAtlas(positionSTS);
	#endif
}
```

这里很显然通过一个DIRECTIONAL_FILTER_SETUP函数来返回一个权重和采样点的列表。然后用一个循环叠加这些周围像素点的采样结果并乘上权重。这个和卷积核的概念其实是一致的，类如什么高斯卷积核，或者是什么soba算子之类的。

而这个DIRECTIONAL_FILTER_SETUP的宏是这么定义的：

```glsl
#if defined(_DIRECTIONAL_PCF3)
	#define DIRECTIONAL_FILTER_SAMPLES 4
	#define DIRECTIONAL_FILTER_SETUP SampleShadow_ComputeSamples_Tent_3x3
#elif defined(_DIRECTIONAL_PCF5)
	#define DIRECTIONAL_FILTER_SAMPLES 9
	#define DIRECTIONAL_FILTER_SETUP SampleShadow_ComputeSamples_Tent_5x5
#elif defined(_DIRECTIONAL_PCF7)
	#define DIRECTIONAL_FILTER_SAMPLES 16
	#define DIRECTIONAL_FILTER_SETUP SampleShadow_ComputeSamples_Tent_7x7
#endif
```

简单来说就是通过一些设置来选择对应的tent filter采样器。下面说的均以SampleShadow_ComputeSamples_Tent_3x3为例子。

然后我就对了一下SampleShadow_ComputeSamples_Tent_3x3这个函数，然后惊奇的发现

```glsl
void SampleShadow_ComputeSamples_Tent_3x3(real4 shadowMapTexture_TexelSize, real2 coord, out real fetchesWeights[4], out real2 fetchesUV[4])
```

这里的定义返回的采样点数组居然长度是4而不是9。然后我又翻回去教程里面找到了这么一个图

这个采样的方式其实应该是这样的：（摘自[https://catlikecoding.com/unity/tutorials/advanced-rendering/depth-of-field/](https://catlikecoding.com/unity/tutorials/advanced-rendering/depth-of-field/)）

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2017.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2017.png)

也就是说本质上这个是一个4个插值器进行加权的采样器，但是需要采样到周围3*3的区域。

所以这里每一个插值器都是由一个2*2的纹素区域加权之后而成的。（因为采样的时候是会有双线性插值，这里不用纠结为什么这个红色点还能够采到样，这个和在CPU里面实现是不一样的。顺便贴一个以前写的笔记：[https://github.com/waihinchan/learning/blob/master/笔记/Pixel outline test.md](https://github.com/waihinchan/learning/blob/master/%E7%AC%94%E8%AE%B0/Pixel%20outline%20test.md)）

所以说SampleShadow_ComputeSamples_Tent_3x3本质上是计算四个采样点加权后的结果。而这个权重是怎么定的呢？

Unity通过使用两个个高度为1.5纹素 底边为3纹素的等腰三角形沿着这个3*3的区域的对角线切开。根据这些三角形所占据的纹素的面积来决定权重。

这里说的有点抽象给一个图，同时给一个Unity源代码里面的注释：

```glsl
// Assuming a isoceles triangle of 1.5 texels height and 3 texels wide lying on 4 texels.
// This function return the area of the triangle above each of those texels.
//    |    <-- offset from -0.5 to 0.5, 0 meaning triangle is exactly in the center
//   / \   <-- 45 degree slop isosceles triangle (ie tent projected in 2D)
//  /   \
// _ _ _ _ <-- texels
// X Y Z W <-- result indices (in computedArea.xyzw and computedAreaUncut.xyzw)
```

这里说的意思是这个底边为3个纹素单位的等腰直角三角形是横跨了4个像素格的。也就是如下：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2018.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2018.png)

同时源码中也提到了：

```glsl
// 3x3 Tent filter (45 degree sloped triangles in U and V)
void SampleShadow_ComputeSamples_Tent_3x3(real4 shadowMapTexture_TexelSize, real2 coord, out real fetchesWeights[4], out real2 fetchesUV[4])
```

注意！接下来的内容可能都是口糊，都是基于一些假设下才成立的。如果这一步开始错了，那么就是我对个tent filter的实现细节理解错了

所以这里我们把这个三角形旋转一下得到如下的一个图：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2019.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2019.png)

当然它也有可能长这样：（如果特别纠结那个底边是不是跨越了4个纹素的话）

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2020.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2020.png)

所以基于上面的这么一个图，我们可以看出来Unity的确也是会把这个区域分割成3*3，同时它会对U和V的方向分别去计算这两个三角形所覆盖的面积。也就是有了如下的内容：

```glsl
real4 texelsWeightsU, texelsWeightsV;
SampleShadow_GetTexelWeights_Tent_3x3(offsetFromTentCenterToCenterOfFetches.x, texelsWeightsU);
SampleShadow_GetTexelWeights_Tent_3x3(offsetFromTentCenterToCenterOfFetches.y, texelsWeightsV);
real2 fetchesWeightsU = texelsWeightsU.xz + texelsWeightsU.yw;
real2 fetchesWeightsV = texelsWeightsV.xz + texelsWeightsV.yw;
```

那么这个以UV作为维度的计算权重方式是怎么样的呢？

关键代码如下：

```glsl
// Assuming a isoceles triangle of 1.5 texels height and 3 texels wide lying on 4 texels.
// This function return the area of the triangle above each of those texels.
//    |    <-- offset from -0.5 to 0.5, 0 meaning triangle is exactly in the center
//   / \   <-- 45 degree slop isosceles triangle (ie tent projected in 2D)
//  /   \
// _ _ _ _ <-- texels
// X Y Z W <-- result indices (in computedArea.xyzw and computedAreaUncut.xyzw)
void SampleShadow_GetTexelAreas_Tent_3x3(real offset, out real4 computedArea, out real4 computedAreaUncut)
{
    // Compute the exterior areas
    real offset01SquaredHalved = (offset + 0.5) * (offset + 0.5) * 0.5;
    computedAreaUncut.x = computedArea.x = offset01SquaredHalved - offset;
    computedAreaUncut.w = computedArea.w = offset01SquaredHalved;

    // Compute the middle areas
    // For Y : We find the area in Y of as if the left section of the isoceles triangle would
    // intersect the axis between Y and Z (ie where offset = 0).
    computedAreaUncut.y = SampleShadow_GetTriangleTexelArea(1.5 - offset);
    // This area is superior to the one we are looking for if (offset < 0) thus we need to
    // subtract the area of the triangle defined by (0,1.5-offset), (0,1.5+offset), (-offset,1.5).
    real clampedOffsetLeft = min(offset,0);
    real areaOfSmallLeftTriangle = clampedOffsetLeft * clampedOffsetLeft;
    computedArea.y = computedAreaUncut.y - areaOfSmallLeftTriangle;

    // We do the same for the Z but with the right part of the isoceles triangle
    computedAreaUncut.z = SampleShadow_GetTriangleTexelArea(1.5 + offset);
    real clampedOffsetRight = max(offset,0);
    real areaOfSmallRightTriangle = clampedOffsetRight * clampedOffsetRight;
    computedArea.z = computedAreaUncut.z - areaOfSmallRightTriangle;
}
```

这里有一个叫offset的东西，其实也就是我们上面的图的一般情况。换言之当我们的采样卷积核并不是准确落在纹素中心的时候，也就意味着我们两个三角形区域会发生偏移。而这个偏移会导致某一些位置的权重发生变化。这里有一个图去描述这种情况：

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2021.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2021.png)

前面也说了计算权重的方法是通过计算这些三角形覆盖的纹素的面积而定的，一旦发生了偏移，三角形覆盖的部分纹素的面积就会发生变化，从而使某一些区域的权重会有所变化。（这里三角形的总面积是不变的，但是三角形覆盖到的每一个纹素的面积是会变化的。）

所以上述的这个SampleShadow_GetTexelAreas_Tent_3x3函数会返回一个四分量，而这个四分量分别代表了这一个三角形所覆盖到的不同纹素的面积。这里描述的仍然是非常抽象。我们直接看代码。

```glsl
    real offset01SquaredHalved = (offset + 0.5) * (offset + 0.5) * 0.5;
    computedAreaUncut.x = computedArea.x = offset01SquaredHalved - offset;
    computedAreaUncut.w = computedArea.w = offset01SquaredHalved;
```

这里如果没有图根本想象不出来好吗！

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2022.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2022.png)

有了这个图就可以看出来，x和w分别代表了左上角和右下角那两个小三角形的区域。这个offset是以中心为定的，我们可以通过简单的三角形求面积求出其中一个也就是(offset + 0.5) * (offset + 0.5) * 0.5。而另外一个由于一边的边长增加或减少，另外一边的边长就必然会增加或减少。而通过一通计算之后可以得出其实另外一个三角形的面积就是等于第一个三角形的面积减去offset的数值。这里可以去验算一下以防我算错了。

同理y和z就是这个大的三角形的面积减去这两个小三角形的面积的一半。一个简单的方法就是把offset=0代入去计算一下就可以了。

我当时演算了很久看到这里我还是懵逼的，就是不太理解为什么要这样拆分成xyzw，而且拆分出来的xyzw也不是一一对应每一个纹素的面积的。可以看到部分区域也是跨越了几个纹素。

然后我作了这么一个图（其实我到这一步是卡住的，我是后来才找到上面的那个3*3tentfilter其实是用四个点来采样的图，如果没有那个图是完全不明白这里为什么要这么做的。）

![shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2023.png](shadows%20b5b3b66d76a94abab8e78f6e8e1605bd/Untitled%2023.png)

由于上面提到了采样的方式是通过4个 2*2的区域来进行加权的。那么当我们用一些颜色区域对这些区域划分一下我们就会发现，即使是同一个对角三角形下，有一些面积的区域也是共享的。而这个xyzw只是为了划分那4个2*2所用到的区域而生。而非单独计算每一个覆盖到的纹素的面积是多少。

比如说左上角的2*2的区域必然会有我们计算的x_v和x_u（u和v是分别计算的）的三角形面积，但是左上角的2*2区域和左下角的2*2区域其实是有重叠的部分，重叠的部分就是第一列第二行和第二列第二行的区域。同理左上角和右上角的2*2区域的重叠部分就是第一行第二列和第二行第二列的区域。

而当发生offset的时候，这些重叠部分的也会发生变化，也就是xyzw四个分量共同控制了这些分割区域的差异。

而这里的代码也印证了我的想法（三角形面积等于2.25，1/2.25约等于0.44444，也就是最终计算出来的xyzw是除以三角形面积。即xyzw显然是局部面积。而最终结果是一个比值。）

```glsl
void SampleShadow_GetTexelWeights_Tent_3x3(real offset, out real4 computedWeight)
{
    real4 dummy;
    SampleShadow_GetTexelAreas_Tent_3x3(offset, computedWeight, dummy);
    computedWeight *= 0.44444;//0.44 == 1/(the triangle area)
}
```

这里就解释了这个xyzw是用来做什么的。然后关于后面的一些计算细节比如如何通过这个四分量计算各个2*2区域的权重（比如合并u和v的面积）以及重新把这些坐标点映射到全局坐标点也就是片段着色器中用于采样的那一个uv坐标点就不描述了。~~（看不下去了）~~

完整的代码可以到unity的github或者随便一个工程文件里面查看，源文件名称叫：ShadowSamplingTent.hlsl