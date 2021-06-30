# SoftRender

This is a soft rasterization renderer written from scratch based on the framework of Yan's class 'GAMERS101'.
The purpose of rewriting this renderer is mainly for better understanding basic CG knowledge and pipeline.
***

## Library

1. The math library [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) is used for calculate vector and matrix.
2. The graphics library [opencv](https://opencv.org/releases/) is used to import textures and display images.The version is 4.5.2.
3. The [obj_loader](https://github.com/Bly7/OBJ-Loader) is used to load obj.
***

## Main Features
- Perspective correct interpolation
- Blinn-Phong shading
- Texture mapping
- Implementing SSAA
- Implementing bilinear interpolation
- Movable rotatable camera
***

## Example
- **Triangles**
<img width="400" height="400" src="https://raw.githubusercontent.com/YukinoKyoU/SoftRender/master/Examples/Triangles.png"/>

- **Tringles with SSAA**
<img width="400" height="400" src="https://raw.githubusercontent.com/YukinoKyoU/SoftRender/master/Examples/TrianglesWithSSAA.png"/>

- **Spot With Normal**
<img width="400" height="400" src="https://raw.githubusercontent.com/YukinoKyoU/SoftRender/master/Examples/spotNormal.png"/>

- **Spot with Blinn-Phong**
<img width="400" height="400" src="https://raw.githubusercontent.com/YukinoKyoU/SoftRender/master/Examples/spotBlinn.jpg"/>

- **Spot with Texture**
<img width="400" height="400" src="https://raw.githubusercontent.com/YukinoKyoU/SoftRender/master/Examples/spotTexture.jpg"/>

- **Mario** (without Texture)
<img width="400" height="400" src="https://raw.githubusercontent.com/YukinoKyoU/SoftRender/master/Examples/Mario.png"/>

- **Neptubne**  (without Texture)
<img width="400" height="400" src="https://raw.githubusercontent.com/YukinoKyoU/SoftRender/master/Examples/Neptune.jpg"/>
