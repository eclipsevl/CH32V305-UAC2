[参考文档](https://www.ti.com.cn/cn/lit/ug/zhcu099/zhcu099.pdf?ts=1772598462062&ref_url=https%253A%252F%252Fwww.bing.com%252F)  
[ES9018k2m参数](https://item.szlcsc.com/datasheet/ES9018K2M/2794554.html?openAI=true&spm=sc.it.xds.aii&lcsc_vid=QVleUgBUFlJdAQZeFQVeUlRWTgJbA1UFRgQKBAUFRQUxVlNRRVRXUVZWTlhZUTsOAxUeFF5JWBYZEEoEHg8JSQcJGk4%3D)

## IV转换器

![alt text](docs/image.png)  
es9018k2m峰值输出电流为 3.783/2 = 1.8915mA  

![alt text](docs/image2.png)  
opa1678线性电压位于 +-0.8v  

![alt text](docs/image3.png)  
我将使用电荷泵产生+-5v，考虑到电压波动，我将运放单个差分通道输出限制在3~4v左右  
因此iv转换器上的反馈电阻最低为 $\frac{3v}/{1.8915mA} = 1586.04om$  
什么几把数字，取个好点的比如2kom  
峰值电压为 2kom * 1.8915mA = 3.783v，离非线性电源柜还有一段距离，可以  
`注：这里电阻越大放大系数越高越有利于减小后级放大的噪声`

![alt text](docs/image4.png)  
copy一下，计算一下电容为
$$C <= \frac{1}{2\pi(2000)(458075)} = 173.32pF$$

## Vbias

![alt text](docs/image5.png)  
![alt text](docs/image6.png)  
![alt text](docs/image7.png)  
$$Vbais = 1.65*(2000/806) / (1 + 2000/806) = 1.17605132V$$
AI推荐  
![alt text](docs/image10.png)  
$$C >= \frac{1}{2\pi(R1)||(R2)} = ?$$
第一个是 1.94714318uF  
第二个是 1.225134613uF  
第三个是 0.664673581uF  

## SGM差分放大器

![alt text](docs/image8.png)  
满量程差分放大器输入为 3.783*2 = 7.566v = 5.34996991Vrms  
A = 1.228Vrms / 5.34996991Vrms = 0.229534001  
$$A = \frac{R2}{R5} = \frac{R11}{R7}$$

| 精度级别 | $R_{in}$ (R5, R7) | $R_f$ (R2, R11) | 实际增益 $A$ | 相对误差 | 备注 |
| --- | --- | --- | --- | --- | --- |
| **1% (E96)** | **10.0 kΩ** | **2.32 kΩ** | **0.232** | +1.07% | 最常用、易购组合 |
| **1% (E96)** | **44.2 kΩ** | **10.2 kΩ** | **0.2307** | +0.54% | 阻值稍大，适合降低前级负载 |
| **0.1% (E192)** | **10.0 kΩ** | **2.29 kΩ** | **0.2290** | -0.23% | 非常接近目标值 |
| **0.1% (E192)** | **42.2 kΩ** | **9.65 kΩ** | **0.2286** | -0.37% |  |
| **0.1% (E192)** | **51.1 kΩ** | **11.7 kΩ** | **0.2289** | -0.25% |  |


![alt text](docs/image9.png)  
$$C=\frac{1}{2\pi(R2)(458075)} = ?$$

### 1% 精度电阻组合 ($f_c = 458,075\,\text{Hz}$)

| 电阻组合 ($R_{in}$ / $R_f$) | 反馈电阻 $R_f$ | 计算电容值 $C$ | 推荐标准电容值 (皮法) |
| --- | --- | --- | --- |
| **10.0 kΩ / 2.32 kΩ** | $2.32\,\text{k}\Omega$ | $149.7\,\text{pF}$ | **$150\,\text{pF}$** |
| **44.2 kΩ / 10.2 kΩ** | $10.2\,\text{k}\Omega$ | $34.0\,\text{pF}$ | **$33\,\text{pF}$** 或 **$36\,\text{pF}$** |

### 0.1% 精度电阻组合 ($f_c = 458,075\,\text{Hz}$)

| 电阻组合 ($R_{in}$ / $R_f$) | 反馈电阻 $R_f$ | 计算电容值 $C$ | 推荐标准电容值 (皮法) |
| --- | --- | --- | --- |
| **10.0 kΩ / 2.29 kΩ** | $2.29\,\text{k}\Omega$ | $151.7\,\text{pF}$ | **$150\,\text{pF}$** |
| **42.2 kΩ / 9.65 kΩ** | $9.65\,\text{k}\Omega$ | $35.9\,\text{pF}$ | **$36\,\text{pF}$** |
| **51.1 kΩ / 11.7 kΩ** | $11.7\,\text{k}\Omega$ | $29.7\,\text{pF}$ | **$30\,\text{pF}$** |
