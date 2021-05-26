# Lit

**팀페이지 주소** -> https://kookmin-sw.github.io/capstone-2021-6

## 목차
1. ### 프로젝트 소개

2. ### 소개 영상

3. ### 팀 소개

4. ### 사용법

5. ### 개발


### 1. 프로젝트 소개


#### Voxel Cone Tracing을 활용한 렌더러


특정 하드웨어 유닛(CUDA Core, Tensor Core, RT Core)을 사용하지 않고도 비슷한 효과를 낼 수 있는 렌더러


사실적인 렌더링을 위해서는 Global Illumination, Shadows, Ambient Occlusion, Reflection, Caustics를 이용하여
렌더링을 해야한다.


우리는 그 중 Global Illumination효과를 내기 위하여 Voxel Cone Tracing이라는 기술을 공부하고 우리의 렌더러에 적용하였다.

### 2. 소개 영상

[![Team6 Lit 소개영상](http://img.youtube.com/vi/NRaRvLj9sr8/0.jpg)](https://youtu.be/NRaRvLj9sr8?t=0s) 

### 3. 팀 소개
<img width="200" alt="곽상열" src="https://user-images.githubusercontent.com/59731956/113509034-5f32ee00-958e-11eb-9a82-ede3401554eb.jpg">



곽상열

Student ID : 20151769

E-Mail : sangjjang96@kookmin.ac.kr

Role : 팀장, 프레임워크 개발

Github : [@sangjjang96](https://github.com/sangjjang96)



<img width="200" alt="양교원" src="https://user-images.githubusercontent.com/59731956/113509058-7a9df900-958e-11eb-8fe8-fa0a8f61b262.png">



양교원

Student ID : 20171647

E-Mail : yangkyowon@kookmin.ac.kr

Role : 팀원, 알고리즘 연구 및 개발

Github : [@SeolYang](https://github.com/SeolYang)

### 4. 사용법

```
UI 켜기 / 끄기 : U
Camera Movement : W A S D
Light Movement : UpArrow DownArrow LeftArrow RightArrow
Enable CamPath : C
Enable Light Roation : L

Change RenderMode : LeftBracket RightBracket
Change Scene to Sponza : F5
Change Scene to CornellBox : F6
```

### 5. 개발

협업 규칙

```
Code Convention

- 매크로 : Uppercase Snake
- 함수명, 클래스, Enumerator, Enumerator Element, Constant : Pascal
- 멤버변수 : Hungarian
- 로컬변수 : Camel
```

