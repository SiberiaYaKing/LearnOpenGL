# 笔记：我觉得比较迷惑的地方...

####1. 法线矩阵为什么是顶点变换矩阵的逆转置矩阵？
* A空间与B空间的法线N与切线T分别设为Na和Ta，Nb和Tb，一定满足Na · Ta = 0，Nb · Tb = 0。A到B空间的顶点变换矩阵为Ma->b，设法线矩阵为G，则有: 
   * (其中："·"表示点乘，"t()"表示转置，"*"表示矩阵乘法，"-"表示逆矩阵，"E"表示单位矩阵) 
   * Nb · Tb = (G * Na) · (Ma->b * Ta) 
   * = (G*Na) · t(Ma->b * Ta) (点乘转化为矩阵乘法，稍微动脑可自证) 
   * = Na * (G * t(Ma->b)) * t(Ta) = 0 
   * 由于 Na · Ta = Na * t(Ta) = Na * E * t(Ta) = 0 
   * 可知当G * t(Ma->b) = E的时候，Nb · Tb = 0成立 
   * 因此G = - t(Ma->b) 
* 也就是说，法线矩阵是顶点变换矩阵的逆转置矩阵！
* 值得一提的是，当Ma->b为正交矩阵的时候，t(Ma->b) = - Ma->b，此时G = Ma->b
* 注意：当Ma->b为等比缩放或旋转矩阵的时候，Ma->b为正交矩阵 
* 这也正是为什么非等比缩放矩阵变换法线会出现错误的结果，此时G != Ma->b，需要求Ma->b的逆转置矩阵才能得到G

## 2.怎么描述格拉姆--施密特正交化过程？
* 格拉姆--施密特正交化的计算过程是：已知相交于某点o的两条向量x与q，其中q为辅助向量，通过叉乘算出另外两条与x正交的向量y与z，个人感觉就是子坐标系在世界空间的"姿态"吧。

## 3.坐标点相减相加得到的向量方向怎么判断
* 首先弄清一个概念，点和向量的表示方法是一样的，所以点的加减结果可以是点也可以是向量，讨论表示向量的情况，设点a点b： 
    * a-b :把b看作(0,0,0) a看作(1,1,1)，减后还是(1,1,1) 可知方向是b->a
    * a+b :把b看作(1,0,0) a看作(0,1,0)，加后变成(1,1,0) 此三点与(0,0,0)围城平行四边形，向量方向就是从(0,0,0)出发的对角线方向

## 4.phong光照公式的向量方向总是搞错？？
* 漫反射部分：因为法向量的方向是固定的，为了得到正确的法向量与光照方向的夹角余弦值，光照方向要做出调整，因此其方向与现实相反
* 高光部分：说到底还是因为法向量是固定的，glsl的reflect函数可以计算光照的反射向量，reflect函数计算的反射如图所示"↘↑↗"，reflect传入的参数是"↘"(入射光向)与"↑"(反射轴即法向量)，返回值是"↗"(反射方向)。因为参数"↑"的位置固定，为了返回值是"↗"，所以"↘"是由一些处理得到的：由于前面计算漫反射的时候把光照方向反过来了("↖")，-"↖"="↘"，以此来纠正光向。由于reflect计算出的"↗"是从片段指向外面，为了得到正确的反射方向与视角方向的夹角余弦值，视角方向也会与现实相反

## 5.坐标轴的手性和叉乘结果方向以及面法线的计算
* 确定手指的坐标轴：伸展食指和大拇指摆出手枪状，食指y轴，大拇指x轴，一定要记住这两个，然后向前伸出中指就是z轴了
* 然后就是记住，右手opengl,左手directx
* 叉乘结果方向: 螺旋定则，即AxB ，四指向外代表A向量，手掌到手腕代表B向量，四指向掌心弯曲，伸出大拇指，大拇指方向即为叉乘方向，opengl用右手，directx用左手
* 面法线的计算：用于culloff判定，三角形面三顶点可以形成两个相交向量，它们的叉乘方向就是法线的方向，所以才说面法线方向也遵守螺旋定则，本质也是叉乘

## 6.Assimp的数据结构图
* 我感觉可以把Scene理解为node的寻址器，node对应的所有资源都可以通过索引来获取。
![](https://learnopengl-cn.github.io/img/03/01/assimp_structure.png)

## 7.纹理的坑
* 单个纹理的情况下，它的采样器(sampler=0)与纹理单元(GL_TEXTURE0)是有默认值的，不用手动设置，在多纹理的情况下则需要激活纹理单元(glActiveTexure)，并设置采样器(shader.setInt("sampler",value))。
* 纹理采样器为空的情况下，glsl的纹理采样函数(texture)会采样失败，而且这种失败是无法检测到，只能通过在外部传入一个bool值到着色器，以此来判断采样器是否为空。

## 8.dll目录的设置
* 本仓库的dll相对目录为：
- path = ..\\..\\OpenGLPackages\\dlls\\dlls_x64 
- path = ..\\..\\OpenGLPackages\\dlls\\dlls_x86
* 在单个项目属性->配置属性->调试->环境中设置
* 由于这个仓库的项目都是debug开发模式，所以可以将这两个目录配置到系统的环境变量中，这样项目就能引用到dll
* 2021/4/9 之后 ：：注意！！！！！！，项目做了通用化处理，生成会自动拷贝dll，不用手动设置了


## 9.imgui 1.60 版本的坑
* 使用imgui 1.60会使glfw的鼠标滚轮回调(glfwSetScrollCallback)失效

## 10.为什么渲染出的草有边框？？
* 注意，当采样纹理的边缘的时候，OpenGL会对边缘的值和纹理下一个重复的值进行插值（因为我们将它的环绕方式设置为了GL_REPEAT。这通常是没问题的，但是由于我们使用了透明值，纹理图像的顶部将会与底部边缘的纯色值进行插值。这样的结果是一个半透明的有色边框，你可能会看见它环绕着你的纹理四边形。要想避免这个，每当你alpha纹理的时候，请将纹理的环绕方式设置为GL_CLAMP_TO_EDGE：
- glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
- glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

## 11.对天空盒的优化
* 优化前：先渲染天空盒，再渲染场景物，但这样做，先渲染天空盒的时候，片元着色器会把所有像素的颜色计算出来，然后再渲染场景物的时候，场景物在天空盒前面 ，片元着色器又重复计算了那些像素，效率不高
* 优化方案：先渲染场景物，保存场景的深度缓冲，最后渲染天空盒的时候，将天空盒的深度变为最大值1.0(即最远)，这样子可以确保场景物占据的像素，在渲染天空盒不会被重复渲染。可以理解为提前对场景物深度测试。
* 要将天空盒深度变为1.0，有一个狡猾的办法，因为透视除法实在顶点着色器运行之后执行的，所以在顶点着色器将gl_Position的xyzw的z分量的值变成w的值,这样经过透视除法之后，z分量的值就变为w/w=1.0。即深度值变为1.0。

## 12.天空盒的观察矩阵(view matrix)为什么要取3x3？
* （待续....