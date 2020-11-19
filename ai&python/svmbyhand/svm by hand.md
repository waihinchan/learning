# svm by hand

Date: Apr 15, 2020
Tags: ongoing

---

## 手工推导

### STEP1:

下面我们来手工推导一下SVM公式以及原理。

首先SVM全称是Support Vector Machine， 中文名字是支持向量机。具体的介绍可以百度，包括历史啥的。

这里推导的前提是所有数据都是线性可分的。这个是什么意思呢我们先来看一个图：

![svm%20by%20hand/Untitled.png](svm%20by%20hand/Untitled.png)

这个图里面的数据很明显都是线性可分的，意思就是我们可以直接在这两堆不同的数据之间，画一条线，把他们分开。所以我们有如下这么个图：

![svm%20by%20hand/Untitled%201.png](svm%20by%20hand/Untitled%201.png)

如果我们能找到这么一条线，能够很好的把这两堆数据给切割开，那当我们有新的数据（特征集）进来的时候，我们就能够很好对新的特征集进行分类。

注意这里特征集指的是这些不同颜色的XX。

而x1和x2则是他们的特征。意思是如果我们有2个以上的特征，那就是3维甚至更高。我们后面会回过头来讲一下这个问题。

所以我们的SVM的目标就有了：

目标：找到一条直线，完美的把两组数据集给切割开。

那么问题来了，你看上面的图，你是否有很多问号。这种线我能给你整一百条。对吧，在这个范围内，我们的线是无限的，只要能切割开他们就完事了。所以我们稍微修改一下我们的目标如下：

目标：找到一条**最好**的直线，完美的把两组数据集给切割开。

那么回过头来说一下，我们现在这个是二维的，那如果是三维（意思是有3个特征），那我们这个空间怎么表示呢？ 比如说我们通过SVM去划分西瓜的品种好坏，那么西瓜有3个特征：

1. 颜色
2. 大小
3. 声音（拍下去的声音）

那此时我们就有了3个特征，那我们就有了如下这个图：

![svm%20by%20hand/Untitled%202.png](svm%20by%20hand/Untitled%202.png)

那这个时候，我们的目标就变成了：

目标：目标：找到一条**最好**的平面，完美的把两组数据集给切割开。

也就是这样：

![svm%20by%20hand/Untitled%203.png](svm%20by%20hand/Untitled%203.png)

那如果是四维呢？五维呢？在现实问题中我们会发现会有超过3个以上的特征，而当发生这种情况时我们没有办法想像出这么一个状况。所以实际上对于这个能够完美分割开两组数据的“东西”，我们称之为超平面，也就是hpyer plane。

所以我们的目标是

目标：目标：找到一条**最好**的超平面，完美的把两组数据集给切割开。

---

### STEP2:

接下来我们就来推导，如何定义这个超平面是最好的。

我们回到二维的例子：

![svm%20by%20hand/Untitled%204.png](svm%20by%20hand/Untitled%204.png)

上图有3条线，我们认为，2 和 3这两条线是和两组特征集相切的（或者说特征集的最外围就落在这根线上）。 那2 3这两条线的中间，即他们的宽度/2， 也就是第一条线，就是我们要找的那条线。

1线被称为决策边界，而23则被称为支持向量。

我们可以看到，当符合这个条件时，我们的目标很显然是让决策边界，到两边的距离d最大。

此时决策边界距离两组数据最远，而且在一定程度上不会受到数据的扰动（比如我把特征集稍微移动一下，不会触碰到我们的决策边界。）

所以现在我们的目标就转变成：

让决策边界距离两组特征集距离最远，即上图中的d最大

由于我们的这条线或者说这个超平面他是线性可分的，那么很显然它是符合这个表达式的：

f(x) = w · x + b

注意这里是点乘，即[内积](https://baike.baidu.com/item/%E7%82%B9%E7%A7%AF/9648528?fromtitle=%E5%86%85%E7%A7%AF&fromid=422863&fr=aladdin)。但是这里不懂没所谓，因为计算机会帮我们完成内积的计算。

这里w和b就是我们平时经常出现的 weight 和 bias。 我们的目标就是，找到 w 和 b，同时让他满足我们上面的目标，也就是d最大。

而决策边界这条线则是 w · x + b = 0 ； 如下图所示

![svm%20by%20hand/Untitled%205.png](svm%20by%20hand/Untitled%205.png)

同时我们认为，所有落在决策边界左边的，都大于0，落在决策边界的右边的，都小于0。

这样我们的数据集（如果符合的话），就能很好地被这条线所分割开了。

那此时我们称红色的为+1类（图错了），橙色的为-1类。

然后我们既然是找到d的距离最大，那么我们可以发现其实中间的这条决策边界，是由左右两边的支持向量平移得来的，那么我们就可以认为，我们要找到的d最大等同于我们要找到的2d最大，意思就是让两个支持向量之间的距离最大。

这么做是方便我们在下面推导的时候可以用到三角函数。

![svm%20by%20hand/Untitled%206.png](svm%20by%20hand/Untitled%206.png)

那我们既然有了两个支持向量：w·x+b>0和 w·x+b<0是对应我们的-1类和+1类的，那么我们可以认为，支持向量的表达式为：

**w·x+b = k 和 w·x+b = -k**

其中k代表，决策边界向左和向右平移了K个单位。（平移这个是高中知识不多赘述）

那么我们对这两个表达式同时*1/k

那么我们就得到了如下：

**w/k · x + b/k = 1 和 w/k · x + b/k = -1**

然后我们可以把 w/k 和 b/k 看作一个整体，则把他们看作一个新的 w 和 b，则有了如下的表达式：

![svm%20by%20hand/Untitled%207.png](svm%20by%20hand/Untitled%207.png)

所以我们现在有3个表达式：

![svm%20by%20hand/Untitled%208.png](svm%20by%20hand/Untitled%208.png)

然后我们在平面上面画个图，把这些线都画出来，如下图所示：

![svm%20by%20hand/Untitled%209.png](svm%20by%20hand/Untitled%209.png)

我们在两个SVM上面取一个数（随便取，只要在这条线上面即可）

然后对x1 和 x2 进行连线， 从x1 到 橙色线（即3线）作一条垂线，这个垂直的距离就是我们要找的2d，即距离，要让它最大。

然后我们让**w/k · x + b/k = 1 和 w/k · x + b/k = -1 相减，**

我们可以得到 w · (x1-x2) = 2

而我们的这个(x1-x2)，则是两个向量相减，那刚好就等于x1点到x2点的连线，也就是蓝色的那条虚线。

从而刚好，我们的距离d也就是那条垂线（x1到对面的垂线或者x2到对面的垂线），和角度θ还有那根蓝色虚线，刚好组成了一个直角三角形

我们来拆解一下，w · (x1-x2) = 2 ，注意这里是点积。而||w||则是范数（可以去百度一下范数和模长的关系）

等于||w|| * ||x1-x2||* cosθ = 2

那因为我们前面说了，蓝色虚线和上面的垂线组成了一个直角三角形，那么我们就得到了

d = ||x1-x2|| * cosθ  

这个d就是那条垂线，也就是我们的距离。

所以我们就得到了

||w|| * d = 2

那 d = 2    /   ||w||

所以我们的目标就是：

**max**    （    2    /   ||w||   ） 这个表达式

所以我们就有

**min**      ||w||

因为||w||的范数是分母，所以我们必然要最小化它。

然后有一个比较容易让人疑惑的点在于，一般情况下我们会写成

**min** 1/2 ||w|| ^ 2

为什么这么写呢，可以看这

[为什么最大化1/||w||与最小化1/2||w||^2是等价的？](https://www.zhihu.com/question/68394029)

![svm%20by%20hand/Untitled%2010.png](svm%20by%20hand/Untitled%2010.png)

---

### step3

最终我们就有了凸优化问题，什么意思呢，我来写一下我们现在的目标应该是这样的：

首先我们要满足一个条件，则对于所有的-1类，我们都有  **w·x + b  ≤ 1** 对于所有的+1类，我们都有 **w·x + b ≥ 1** 

而我们的目标则是，最小化||w||（一般写成 1/2 ||w|| ^ 2）

但是我们还可以在作进一步简化。

那就是，我们其实在机器学习中，我们知道我们除了有特征集以外，我们还有对应的label(这是监督学习，也有半监督学习或无监督学习的，监督学习就是对应结果label)，那我们这里的label是什么呢？

很明显我们的结果就是把x代入我们的表达式之后得到的结果。那么我们在上文中说了，所有的-1类代入去都会小于等于-1，所有的+1类代入去都会大于等于+1，那么我们可以把表达式写成:

**yi * (w·x + b )≥ 1** 

其中yi对应的是label，也就是结果。

这个很好理解，如果是-1类的话，那么**yi必然小于-1**，同时表达式**(w·x + b )也必然小于-1，那么负负得正结果就大于等于1。**

同理+1类也是一样的。

所以我们最后的问题就变成：

目标：最小化1/2 ||w|| ^ 2
条件：所有的特征集（注意这里说的是所有，不再分-1类和+1类了)都符合：**yi * (w·x + b )≥ 1** 

那么到这里我们的SVM手工推导就完事了。不过注意这个仅仅是针对线性可分的数据集。下面看代码部分：

---

## 代码

    import matplotlib.pyplot as plt
    from matplotlib import style
    import numpy as np
    style.use('ggplot')
    
    class Support_Vector_Machine:
        def __init__(self, visualization=True):
            self.visualization = visualization
            self.colors = {1:'r',-1:'b'}
            if self.visualization:
                self.fig = plt.figure()
                self.ax = self.fig.add_subplot(1,1,1)
        

首先一开始我们造一个手工的SVM类。

这里还有一些是关于 初始化的以及可视化的参数调整，可以暂时忽略。

     def fit(self, data):
            self.data = data
    
            opt_dict = {}
    
            transforms = [[1,1],
                          [-1,1],
                          [-1,-1],
                          [1,-1]]
    
    
            all_data = []
    
    
            for yi in self.data:
    
                for featureset in self.data[yi]:
    
                    for feature in featureset:
    
                        all_data.append(feature)
    
            self.max_feature_value = max(all_data)
    
            self.min_feature_value = min(all_data)
    
            all_data = None

fit函数是比较关键的。

第一步是有一个字典，叫优化字典，这个是用于储存我们所有符合约束条件的w和b组合，最后要比较他们的范数大小。

transforms用于变换方向，这是一种优化方向，也就是当我们开始逼近的时候，如果小于0我们就倾向于它的方向发生了变化，此时我们可以直接跳过逼进，然后让w变换方向再次逼近

all_data这一步是用于找到一个初始值w（其实没有也可以 我们可以直接随便取一个数），当然除此之外他们还把获得的数用于可视化了 所以也不是完全没用

所以这里的迭代就是把所有特征展开成一维，然后找到最小值和最大值。

    step_sizes = [self.max_feature_value * 0.1,
                          self.max_feature_value * 0.01,
                          # point of expense:
                          self.max_feature_value * 0.001,
                          ]
            #这个是步长
    
            # extremely expensive
            b_range_multiple = 2
            # we dont need to take as small of steps
            # with b as we do w
            b_multiple = 5
            #因为b是要尽可能大的
            latest_optimum = self.max_feature_value*10

这一步的作用是  stepsize 等于 我们上面取到的最大值 然后把这个最大值分别乘以0.1 0.01 0.001 这个就是步长，和我们之前做的SVM其实还是很相像的

b范围乘数 和 b乘数 看后面

到这一步 我们就有了latestopt 等于我们找到的所有数据中的最大值再*10 我们可以试一下改这个数值

其实这个初始的w是多大不重要 应该说这个你无论取什么数他最后都能到达 因为本质上他是运用算力 在计算所有符合这个条件下的 w 同时比较他们的范数大小 我们看看后面就懂了

    for step in step_sizes:#这里基本上就是四个
    
                w = np.array([latest_optimum,latest_optimum])
                # 让w 等于 latest_optimum 一开始是用了一个10 有点大- -
    
                # we can do this because convex
                optimized = False

#这一步是 我们前面有4个优化步长（还是3个 也就是我们一开始 迈步子很大一步，然后逐渐小步子 你可以测试一下把步子直接是0.01那个 他会运行非常非常久 哪怕你久只有那么3个数

所以我们这会运作3次 根据你的步长 你还可以家一个0.001

每次我们都会清空self.w 其实也不是清空 它在后面会把我们数据储存一下 然后根据上一次的结果重新初始化一个新的w

这里就是我们有了一个w向量，那我们第一次无疑是取的非常大的那个数，也就是我们在所有特征中找到最大的数再*10，把它作为起始w

然后我们有一个优化与否的bool，这个意思是一旦找到了符合条件的内容，我们就不优化了。找符合条件内容看下面

    while not optimized:
    
                    for b in np.arange(-1*(self.max_feature_value*b_range_multiple),
    
                                       self.max_feature_value*b_range_multiple,
                                       step*b_multiple):
    
                        for transformation in transforms:
    
                            w_t = w*transformation
    
                            found_option = True
    
                            for i in self.data:
                                for xi in self.data[i]:
                                    yi=i
                                    if not yi*(np.dot(w_t,xi)+b) >= 1:
                                        found_option = False
                                        break
                            if found_option:
                                opt_dict[np.linalg.norm(w_t)] = [w_t,b]
    
    
                    if w[0] < 0:
                        optimized = True
                        # print(w[0])
                        # print(w[1])
                        print('Optimized a step.')
                    else:
                        w = w - step

这一步的意思是什么呢，首先我们上面让optimized等于false 也就是符合了我们这个while的条件。那就让我们一直一直的在这个while条件下循环，直到optimized为true

首先我们看到我们先要找一个b符合条件的

所以我们用b来循环 至于b的范围从哪里取呢 我们可以看到 从负的最大特征值*一个b的范围乘数，到正的最大特征值*一个b的范围乘数，且步长是当前step下（这意味着我们到后面步子会迈得越来越小）*b的恒定步子乘数

意思就是 哪怕步子为0.01 b也有最起码保证0.05的步长（因为我们上面设置的是5）这个意思就是 我们第一次循环0.1 的情况下

我们的b的取值范围会在-1*maxvaluefeature*2 到 1*maxvaluefeature*2 之间 步长为2的倍数 那就是这个步骤下我们找b会起码找到 4*maxvaluefeature/0.5（0.05 0.005） 组数

这意味着 范围是不会变化的 但是一开始找到的b的数据会不多 但是后面随着步长变小 我们步长作为被除数的长度会逐渐增大

然后我们再用矩阵方向变换来进行迭代，意味着这里是我们在找方向。方向的解释看前面，但是我们有4个方向那就是迭代4次

首先我们对w_t进行转换 也就是如果你原来是 10 ， 10 那你现在就是 -10，-10 或者 10，-10 这样子转换个方向 尽管他们的范数是一样的。因为我们求的是范数最小 但是同时我们还要考虑方向

foundoption = true 这个布尔值的意思是我们在初期要复位 然后如果我们找到一个不符合条件的 就直接跳出这个循环

然后我们开始取数，在原始数据集里面取数，这个时候我们按照特征集为最小单位进行取数

我们首先获得label 也就是 yi 也就是 你是正1还是负1 然后我们把它放到约束条件下进行验证：

验证 在当前的w_t 取这么一个数值下 我们是否有 所有的特征集点乘这个w + b 都符合我们这个条件，一旦有一个不符合 直接break 同时我们让foundoption这个开关关上 意味着马上结束这次的w_t和b组合

然后如果found option 一直到整个循环都没被关上（意思是所有的特征集都符合这个w_t和b的话）我们就储存一下这个组合到我们很开始储存在的opt_dict中

那这里的循环就结束了。什么意思呢。整个流程是这样的

我们的目标是找到w和b符合约束条件 同时尽可能小

所以我们首先要让w和b符合约束条件，然后再通过调整步长去慢慢缩小范围，让我们的w范数尽可能的小

所以很显然，我们一开始是通过步长去控制，也就是大范围搜索，再到地毯式搜索。一开始尽可能找到尽可能多的 符合约束条件的w和b的组合

所以：

首先，我们是有3个步长的迭代，意思是我们一开始要迈大步子去优化

然后我们会有一个名字叫优化开关的内容。这个优化开关决定了你是否能进入下一个步长。意思就是 我们在这个大范围搜索的胜利条件是，你需要找到很多 符合约束条件的w和b的组合，一旦你找到了，我才说你可以进入下一个步长。

下一步我们开始找B，因为b的取值是尽可能大 我们的步子可以迈长一点，也就是我们先在一定范围内 地毯式搜索b

下一步我们先倒入矩阵变换：

这一步的目的是，我们要转换方向，这里等下解释

然后下一步我们开始找w，w一开始会给一个比较高的初始值，然后用这一个w（注意这里的w的方向已经被转换了，这个过程要经历4次，也就是每个w都要经历4次的转换），来验证我们上上步中的b，如果我们把b的范围写成数组，你可以看作一个排列组合问题，也就是我们用w_t 与b[0]、b[1]、b[2]`````一直到b[-1]来进行排列组合

所以其实我们单个w_t要经历 4（这个是方向转换）* b.length 这么多次的验证

然后下一步就是提取每一个数据中的特征集进行验证，是否符合这个条件，如何不符合条件，马上跳出去，要不换方向继续试，要不就换b的数值继续试

然后再下一步

    if found_option:
                                opt_dict[np.linalg.norm(w_t)] = [w_t,b]

这个我们要注意它仍然在方向变换的范围下的，这意思就是说 我们在某一个方向下，找到了这个特定的w_t和b的组合后，我们就要把它储存起来。

那由于我们上面的for循环是先运行的，所以这一步不会收到上一步的这个foundoption的bool开关所影响，因为我们要先走约束条件的验证，一旦约束条件验证失败，foundoption就会变成false ，那意味着这个储存就不会执行

然后到这里我们就结束了这个b的循环。到这里我们一共经历了 n个b*4*一个w_t这么多次，其实这一步是在尝试b和不同方向的w_t，但是我们仍然没有跳出while optimized这个条件，我们接下来看下面的部分：

    if w[0] < 0:
                        optimized = True
                        # print(w[0])
                        # print(w[1])
                        print('Optimized a step.')
                    else:
                        w = w - step

这一步的意思非常疑惑，其实它应该是这么理解的。我们每一次尝试n个b与4个方向的w_t排列组合之后，我们要让，w_t减去步长。（不然步长就没意义了啊)。

我们有没有发现，上面的循环中，一直是一个特定的确定数值的w_t和n个不同的b进行排列组合 ，而w_t本身是没有变化的。意思是，如果我试编了所有的b都和现在就这个w_t都不符合约束条件，那咋办呢，我们让w-setp，也就是maxvalue*0.01 或者0.001 这种，意思是就是，我们让w_t减少一点点，再重新和n个b再搭配一次。（w_t好累）

好了那么试到啥时候才算完呢，当w[0]<0的时候。这个是什么逻辑呢。我们随便拿个数试验一下。 假设我们初始的w_t 是 100。 好现在我们让w_t 和 n个b尝试了排列组合去验证约束条件，可能这中间出现了一些，符合我们条件的数。但是这仅仅是100啊，那99呢 98呢 99.9呢？ 好 ，所以我们接下来要让w 进行尝试。

那就是我们让w 稍微减少一点点 来看看，那就是99.9。 然后又重复了上面的排列组合的过程。那就是说我们的w_t是会一直减少的。然后我们的w_t会一直减少到0，甚至小于0。

那为什么我们不永远减下去呢，因为我们再减下去 我们就等于负数了啊。那你可能会说，负的w_t不是也有可能符合情况的吗？但是你要知道，我们有一个额矩阵变换，就是方向的转换 所以假设 当我们从100开始起步 到 -1 然后一直减到 -100 的时候 我们是不是就等于让 100*-1 了？然而这一步我们已经做过了，在矩阵变换里面。所以我们就不需要了。所以我们这个w[0]有点像是绝对值，当绝对值小于等于0的时候，就可以去下一个步长了。所以我们可以看到 w_t的取值范围会在-maxvalue 到 maxvalue 之间

    norms = sorted([n for n in opt_dict])
                #||w|| : [w,b]
    
                opt_choice = opt_dict[norms[0]]
                self.w = opt_choice[0]
    
                self.b = opt_choice[1]
                latest_optimum = opt_choice[0][0]+step*2

然后这一步就是 先对这一次大步子下所有符合约束条件的w_t 和 b进行排列组合 然后 我们可以看到让self.w 和 self.b 取w范数最小的那个组合。

然后我们在更新一下参数 意思就是现在我们要从最小范数的那个组合里面找到w，然后让它往回走两步（+step*2），然后我们再一次进行小步子的迭代。

    for i in self.data:
                for xi in self.data[i]:
                    yi=i
                    print(xi,':',yi*(np.dot(self.w,xi)+self.b))

这里的这一步是跳出了上面的3个step的逼近，也就是说这个是fit的最后一步。这一步其实就是把所有的特征集对应的点乘结果print出来 没啥特别，验证一下这个结果是否大于1

到这里就结束了。

---

## 梯度下降法：


讲述了这个单门是如何操作的。这里用到拖力的概念，把数值稍微往上拖一下，具体不详细说，反正目的就是为了得到比这个数更大的数

前面巴拉巴拉的一大堆先忽略，从导数角度来说

如果一个*门，直接交换两个数即可得到导数

即x的梯度是y，y的梯度是x

然后我们给一个setp_size非常小的（通常情况下不要太大，但是在这个情况下可以很大，越大结果越好）

x和y各自+之后重新放入*门就可以得到比原数据更大的数值

    var x = -2, y = 3;
    var out = forwardMultiplyGate(x, y); // before: -6
    var x_gradient = y; // by our complex mathematical derivation above
    var y_gradient = x;
    
    var step_size = 0.01;
    x += step_size * x_gradient; // -1.97
    y += step_size * y_gradient; // 2.98
    var out_new = forwardMultiplyGate(x, y); // -5.87. Higher output! Nice.

---

现在我们有两个门，一个+门和一个*门，目的还是要得到比这个数更小的数，这个最大最小的数还是要看你的拖力的方向的（我觉得可能和向量有关？）

![svm%20by%20hand/Untitled%2011.png](svm%20by%20hand/Untitled%2011.png)

这里就是（x+y）*z ，给你一组数，目标要比原来的数大

    var forwardMultiplyGate = function(a, b) { 
      return a * b;
    };
    var forwardAddGate = function(a, b) { 
      return a + b;
    };
    //一个+gate一个*gate
    //这个是gate的组合，就是所谓的复杂电路
    var forwardCircuit = function(x,y,z) { 
      var q = forwardAddGate(x, y);
      var f = forwardMultiplyGate(q, z);
      return f;
    };
    
    var x = -2, y = 5, z = -4;
    var f = forwardCircuit(x, y, z); // output is -12
    //初始结果

这里的思路就是拆解，假设+gate不存在，还是计算*gate。所以我们才把电路写成2个gate并给一个q来储存，输入q到*gate中，这样我们只需要计算q和z的导数，就可以得到比最终整个更大的数，这就回到了我们最初的部分。直接交换q和z就可以了。

问题就在于+gate的导数怎能求呢

看这个初始公式：这个非常重要！！！划重点，万能公式（虽然说简单的还是可以手算的，复杂的就gg了）

![svm%20by%20hand/Untitled%2012.png](svm%20by%20hand/Untitled%2012.png)

这里就是(x+h+y -( x+y))/h  = 1,所以我们就有了如下的公式↓

![svm%20by%20hand/Untitled%2013.png](svm%20by%20hand/Untitled%2013.png)

所以我们现在得到了+gate的两个导数xy的都是1，也就是如果我们想要得到+gate中比原来的数据更大的数的话，我们就这样

    _x = x+ setep_size * 1//1是导数，因为懒得编变量名就直接写了
    _y = y+ setup_size * 1
    out  = *gate(_x,_y)
    //这样你无论啥数都肯定能比这个大，+gate应该是非常简单的

---

当我们把2个gate结合起来的时候他的推导公式如下，

![svm%20by%20hand/Untitled%2014.png](svm%20by%20hand/Untitled%2014.png)

稍微拆解一下，稍微复杂的数学题。

    var x = -2, y = 5, z = -4;
    var q = forwardAddGate(x, y); // q is 3
    var f = forwardMultiplyGate(q, z); // output is -12
    
    // gradient of the MULTIPLY gate with respect to its inputs
    // wrt is short for "with respect to"
    var derivative_f_wrt_z = q; // 3
    var derivative_f_wrt_q = z; // -4
    //这里是如果我们只看q和z的话的确就是对调梯度
    // derivative of the ADD gate with respect to its inputs
    var derivative_q_wrt_x = 1.0;
    var derivative_q_wrt_y = 1.0;
    //如果只看+gate就是1
    
    //重点来了
    // chain rule
    var derivative_f_wrt_x = derivative_q_wrt_x * derivative_f_wrt_q; // -4
    //这里应该是，f相对于x的梯度，也就是还是那个门，一个+一个*，相对于x的导数
    var derivative_f_wrt_y = derivative_q_wrt_y * derivative_f_wrt_q; // -4
    // final gradient, from above: [-4, -4, 3]
    var gradient_f_wrt_xyz = [derivative_f_wrt_x, derivative_f_wrt_y, derivative_f_wrt_z]
    //同上，这时候我们就有了3个导数了，因为这个是三维的
    // let the inputs respond to the force/tug:
    var step_size = 0.01;
    //老方法了
    
    x = x + step_size * derivative_f_wrt_x; // -2.04
    y = y + step_size * derivative_f_wrt_y; // 4.96
    z = z + step_size * derivative_f_wrt_z; // -3.97
    
    // Our circuit now better give higher output:
    var q = forwardAddGate(x, y); // q becomes 2.92
    var f = forwardMultiplyGate(q, z); // output is -11.59, up from -12! Nice!
    //最终的结果就是这样的

所以我们如果有几个变量，有几个门，就要拆解，然后得出每一个维度的导数，然后再输入新的数据到电路门中，我们就可以得到一个复杂电路中比原来小的数据。

这里有两个力，第一个q和z，如果我们看第一个案例中，就是我们要q更小，z更大，等于给q一个减少的力，给z一个增大的力，这样我们就可以得到*gate中更大的数了（你自己想想是这么回事）

我们得到的z其实是让z变大，同时得到的q是让q变小

问题来了，我们的q本身是要变大的，因为addgate得到的结果是要变大，但问题是q相对于f是要变小的，因为它本身是正数，你*负数那肯定要自己变小，才能让最终输出的结果变小，这里不要纠结于我到底是变大是变小，因为这个结果是计算出来的，所以如果我们有一个正数相乘的话，他的结果也会不一样。

所以如果addgate本身是要变大（因为你整个逻辑电路都要求你往变大的导数方向去走），但是这样导致了addgate和q是矛盾的。那怎么办呢，所以就要让+gate响应整个大的逻辑电路的逻辑，也就是让+gate变得更小，因为这样可以让q更小，从而是逻辑电路想要的结果。

所以我们这里要让+gate的xy导数*q相对于f的导数，也就是1*-4 这样我们就有了我们的结果了。

所以*从逻辑电路来说，就是有点像改变你的方向，负负得正，正负得负，正正得正。

最后附一个常用函数的导函数

![svm%20by%20hand/Untitled%2015.png](svm%20by%20hand/Untitled%2015.png)

---

至于更具体的可以看这里

[Understanding the mathematics behind Support Vector Machines](https://shuzhanfan.github.io/2018/05/understanding-mathematics-behind-support-vector-machines/)
